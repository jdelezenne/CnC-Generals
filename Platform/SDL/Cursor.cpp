// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/Cursor.h"
#include "Platform/Paths.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <mutex>
#include <vector>
struct Platform::Cursor {
    std::vector<SDL_Cursor*> frames;
    std::vector<Uint64> durations;
    Uint64 duration = 0;
};
namespace {
std::mutex CursorMutex;
Platform::Cursor* Selected = nullptr;
Uint64 Start = 0;
std::size_t Frame = 0;
bool Changed = false;
}
Platform::Cursor* Platform::LoadCursorFile(const char* path)
{
    IMG_Animation* animation = IMG_LoadAnimation(ReadPath(path).c_str());
    if (!animation) return nullptr;
    auto* cursor = new Cursor;
    for (int i = 0; i < animation->count; ++i) {
        auto* surface = animation->frames[i];
        const auto properties = SDL_GetSurfaceProperties(surface);
        auto* frame = SDL_CreateColorCursor(surface,
            static_cast<int>(SDL_GetNumberProperty(properties, SDL_PROP_SURFACE_HOTSPOT_X_NUMBER, 0)),
            static_cast<int>(SDL_GetNumberProperty(properties, SDL_PROP_SURFACE_HOTSPOT_Y_NUMBER, 0)));
        if (!frame || animation->delays[i] <= 0) {
            if (frame) SDL_DestroyCursor(frame);
            for (auto* previous : cursor->frames) SDL_DestroyCursor(previous);
            delete cursor;
            IMG_FreeAnimation(animation);
            return nullptr;
        }
        cursor->frames.push_back(frame);
        cursor->durations.push_back(animation->delays[i]);
        cursor->duration += animation->delays[i];
    }
    IMG_FreeAnimation(animation);
    if (cursor->frames.empty()) { delete cursor; return nullptr; }
    return cursor;
}
void Platform::DestroyCursor(Cursor* cursor)
{
    if (!cursor) return;
    std::lock_guard lock(CursorMutex);
    if (Selected == cursor) { Selected = nullptr; Changed = true; SDL_SetCursor(SDL_GetDefaultCursor()); }
    for (auto* frame : cursor->frames) SDL_DestroyCursor(frame);
    delete cursor;
}
void Platform::SetCursor(Cursor* cursor)
{
    std::lock_guard lock(CursorMutex);
    if (Selected == cursor) return;
    Selected = cursor;
    Start = SDL_GetTicks();
    Frame = 0;
    Changed = true;
}
void Platform::UpdateCursor()
{
    std::lock_guard lock(CursorMutex);
    if (!Selected) {
        if (Changed) SDL_HideCursor();
        Changed = false;
        return;
    }
    auto elapsed = (SDL_GetTicks() - Start) % Selected->duration;
    std::size_t frame = 0;
    while (elapsed >= Selected->durations[frame]) elapsed -= Selected->durations[frame++];
    if (Changed || frame != Frame) {
        SDL_SetCursor(Selected->frames[frame]);
        SDL_ShowCursor();
        Frame = frame;
        Changed = false;
    }
}
void Platform::MousePosition(int& x, int& y)
{
    float horizontal, vertical;
    SDL_GetMouseState(&horizontal, &vertical);
    x = static_cast<int>(horizontal);
    y = static_cast<int>(vertical);
}
