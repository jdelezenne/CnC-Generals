// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/SystemInfo.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <limits>
#include <sys/resource.h>
#include <sys/utsname.h>
#ifdef __linux__
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#include <mach/mach.h>
#endif
bool Platform::QueryMemoryInfo(MemoryInfo& info)
{
#ifdef __linux__
    struct sysinfo memory{};
    if (sysinfo(&memory) != 0) return false;
    const auto bytes = [unit = memory.mem_unit](std::uint64_t count) {
        return MemorySize(count * unit);
    };
    info.physical = bytes(memory.totalram);
    info.availablePhysical = bytes(memory.freeram + memory.bufferram);
    info.page = bytes(memory.totalram + memory.totalswap);
    info.availablePage = bytes(memory.freeram + memory.bufferram + memory.freeswap);
    struct rlimit limit{};
    const auto maximum = MemorySize(std::numeric_limits<std::uint64_t>::max());
    info.virtualSize = getrlimit(RLIMIT_AS, &limit) == 0 && limit.rlim_cur != RLIM_INFINITY
        ? MemorySize(limit.rlim_cur) : maximum;
    info.availableVirtual = info.virtualSize;
    return true;
#elif defined(__APPLE__)
    const auto bytes = [](std::uint64_t count) {
        return MemorySize(count);
    };
    std::uint64_t physical;
    std::size_t length = sizeof(physical);
    if (sysctlbyname("hw.memsize", &physical, &length, nullptr, 0)) return false;
    vm_statistics64_data_t statistics{};
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    const auto host = mach_host_self();
    vm_size_t pageSize = 0;
    const auto pagesResult = host_page_size(host, &pageSize);
    const auto statisticsResult = host_statistics64(host, HOST_VM_INFO64,
        reinterpret_cast<host_info64_t>(&statistics), &count);
    mach_port_deallocate(mach_task_self(), host);
    if (pagesResult != KERN_SUCCESS || statisticsResult != KERN_SUCCESS) return false;
    xsw_usage swap{};
    length = sizeof(swap);
    if (sysctlbyname("vm.swapusage", &swap, &length, nullptr, 0)) return false;
    const auto available = static_cast<std::uint64_t>(statistics.free_count + statistics.inactive_count) * pageSize;
    info.physical = bytes(physical);
    info.availablePhysical = bytes(available);
    info.page = bytes(physical + swap.xsu_total);
    info.availablePage = bytes(available + swap.xsu_avail);
    struct rlimit limit{};
    info.virtualSize = getrlimit(RLIMIT_AS, &limit) == 0 && limit.rlim_cur != RLIM_INFINITY
        ? bytes(limit.rlim_cur) : MemorySize(std::numeric_limits<std::uint64_t>::max());
    info.availableVirtual = info.virtualSize;
    return true;
#else
#error Unsupported native system information platform
#endif
}
bool Platform::QueryOSInfo(OSInfo& info)
{
    struct utsname os{};
    if (uname(&os) != 0) return false;
    info = {};
    std::sscanf(os.release, "%u.%u.%u", &info.major, &info.minor, &info.build);
    info.extra = os.release;
    return true;
}
const char* Platform::OperatingSystemName(unsigned)
{
#ifdef __APPLE__
    return "macOS";
#else
    return "Linux";
#endif
}
int Platform::TimeZoneBiasMinutes()
{
    const auto now = std::time(nullptr);
    std::tm local{};
    return localtime_r(&now, &local) ? static_cast<int>(-local.tm_gmtoff / 60) : 0;
}
