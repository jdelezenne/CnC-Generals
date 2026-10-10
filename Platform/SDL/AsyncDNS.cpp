#include "Platform/AsyncDNS.h"
#include "Platform/Sockets.h"
#ifdef _WIN32
#include <ws2tcpip.h>
#endif
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_stdinc.h>
#include <atomic>
#include <new>

struct Platform::DNSLookup {
    std::atomic<unsigned> references{2};
    std::atomic<bool> done{false};
    bool succeeded = false;
    char* hostname = nullptr;
    SDL_Thread* thread = nullptr;
};
namespace {
void Release(Platform::DNSLookup* lookup)
{
    if (lookup->references.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        SDL_free(lookup->hostname);
        lookup->~DNSLookup();
        SDL_free(lookup);
    }
}
int SDLCALL Resolve(void* argument)
{
    auto* lookup = static_cast<Platform::DNSLookup*>(argument);
    addrinfo hints{};
    hints.ai_family = AF_INET;
    addrinfo* result = nullptr;
    lookup->succeeded = getaddrinfo(lookup->hostname, nullptr, &hints, &result) == 0;
    if (result) freeaddrinfo(result);
    lookup->done.store(true, std::memory_order_release);
    Release(lookup);
    return 0;
}
}
Platform::DNSLookup* Platform::StartDNSLookup(const char* hostname)
{
    void* memory = SDL_malloc(sizeof(DNSLookup));
    if (!memory) return nullptr;
    auto* lookup = new (memory) DNSLookup;
    lookup->hostname = SDL_strdup(hostname);
    if (lookup->hostname) lookup->thread = SDL_CreateThread(Resolve, "AsyncDNS", lookup);
    if (!lookup->thread) {
        Release(lookup);
        Release(lookup);
        return nullptr;
    }
    return lookup;
}
Platform::DNSLookupStatus Platform::PollDNSLookup(DNSLookup*& lookup)
{
    if (!lookup) return DNSLookupStatus::Failed;
    if (!lookup->done.load(std::memory_order_acquire)) return DNSLookupStatus::Pending;
    SDL_WaitThread(lookup->thread, nullptr);
    const auto status = lookup->succeeded ? DNSLookupStatus::Succeeded : DNSLookupStatus::Failed;
    Release(lookup);
    lookup = nullptr;
    return status;
}
void Platform::CancelDNSLookup(DNSLookup*& lookup)
{
    if (!lookup) return;
    SDL_DetachThread(lookup->thread);
    Release(lookup);
    lookup = nullptr;
}
