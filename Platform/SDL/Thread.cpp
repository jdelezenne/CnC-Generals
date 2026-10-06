// SPDX-License-Identifier: GPL-3.0-or-later
#include "thread.h"
#include "wwdebug.h"
#include "Platform/Threads.h"
#include <SDL3/SDL.h>
#include <cstring>
#include <stdexcept>
#include <string>

namespace {
thread_local ThreadClass* CurrentWorker = nullptr;
thread_local int AppliedPriority = 0;

void ApplyPriority(int priority)
{
    if (priority == AppliedPriority) return;
    // Preserve rejection of legacy values (-3/-4) outside Win32's priority set.
    if ((priority < -2 && priority != -15) || (priority > 2 && priority != 15)) return;
    const auto level = priority < 0 ? SDL_THREAD_PRIORITY_LOW
        : priority == 15 ? SDL_THREAD_PRIORITY_TIME_CRITICAL
        : priority > 0 ? SDL_THREAD_PRIORITY_HIGH : SDL_THREAD_PRIORITY_NORMAL;
    if (!SDL_SetCurrentThreadPriority(level))
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unable to set thread priority: %s", SDL_GetError());
    AppliedPriority = priority;
}
const char* ThreadName(ThreadClass* thread)
{
#ifdef GEN_GAME_ZERO_HOUR
    return thread->Get_Name();
#else
    return "No name";
#endif
}
}

#ifdef GEN_GAME_ZERO_HOUR
ThreadClass::ThreadClass(const char* thread_name, ExceptionHandlerType exception_handler)
    : running(false), ThreadID(0), ExceptionHandler(exception_handler), handle(nullptr), thread_priority(0)
{
    if (thread_name) {
        assert(strlen(thread_name) < sizeof(ThreadName) - 1);
        strcpy(ThreadName, thread_name);
    } else {
        strcpy(ThreadName, "No name");
    }
}
#else
ThreadClass::ThreadClass() : running(false), handle(nullptr), thread_priority(0) {}
#endif
ThreadClass::~ThreadClass() { Stop(); }

void ThreadClass::Invoke_Thread_Function(void* context)
{
    static_cast<ThreadClass*>(context)->Thread_Function();
}
int ThreadClass::Internal_Thread_Function(void* context)
{
    auto* thread = static_cast<ThreadClass*>(context);
    CurrentWorker = thread;
    ApplyPriority(thread->thread_priority.load());
#ifdef GEN_GAME_ZERO_HOUR
    thread->ThreadID = Platform::CurrentThreadId();
    Platform::RunThreadFunction(&Invoke_Thread_Function, thread, thread->ExceptionHandler,
        thread->ThreadName, thread->ThreadID);
    thread->ThreadID = 0;
#else
    Platform::RunThreadFunction(&Invoke_Thread_Function, thread, nullptr, "No name", Platform::CurrentThreadId());
#endif
    thread->running = false;
    CurrentWorker = nullptr;
    return 0;
}
void ThreadClass::Execute()
{
    if (handle && !Is_Running()) {
        SDL_WaitThread(handle, nullptr);
        handle = nullptr;
    }
    WWASSERT(!handle); // Only one thread at a time!
    running = true;
    handle = SDL_CreateThread(&Internal_Thread_Function, ::ThreadName(this), this);
    if (!handle) {
        running = false;
        throw std::runtime_error(std::string("Unable to start thread: ") + SDL_GetError());
    }
#ifdef GEN_GAME_ZERO_HOUR
    WWDEBUG_SAY(("ThreadClass::Execute: Started thread %s, thread handle is %p\n", ThreadName, handle));
#endif
}
void ThreadClass::Set_Priority(int priority)
{
    thread_priority = priority;
    if (CurrentWorker == this) ApplyPriority(priority);
}
void ThreadClass::Stop(unsigned milliseconds)
{
    running = false;
    if (CurrentWorker == this) throw std::logic_error("A thread cannot join itself");
    if (!handle) return;
    const auto start = SDL_GetTicks();
    while (SDL_GetThreadState(handle) != SDL_THREAD_COMPLETE && SDL_GetTicks() - start < milliseconds)
        SDL_Delay(1);
    if (SDL_GetThreadState(handle) != SDL_THREAD_COMPLETE)
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Waiting for thread '%s' to finish after %u ms",
            ::ThreadName(this), milliseconds);
    int status = -1;
    SDL_WaitThread(handle, &status);
    const int res = status == 0;
    WWASSERT(res); // Thread must finish before its owner is destroyed.
    handle = nullptr;
}
void ThreadClass::Sleep_Ms(unsigned milliseconds)
{
    if (CurrentWorker) ApplyPriority(CurrentWorker->thread_priority.load());
    SDL_Delay(milliseconds);
}
void ThreadClass::Switch_Thread() { Sleep_Ms(1); }
unsigned ThreadClass::_Get_Current_Thread_ID() { return Platform::CurrentThreadId(); }
bool ThreadClass::Is_Running() { return handle && SDL_GetThreadState(handle) != SDL_THREAD_COMPLETE; }
