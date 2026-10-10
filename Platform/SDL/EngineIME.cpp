// SPDX-License-Identifier: GPL-3.0-or-later
#include "PreRTS.h"
#include "GameClient/IMEManager.h"
#include "GameClient/GameWindow.h"
#include "GameClient/GameWindowManager.h"
#include "GameClient/Keyboard.h"
#include "Platform/TextInput.h"
#include "Platform/Dialogs.h"
#include "Platform/UTF16.h"
#include <SDL3/SDL_stdinc.h>
#include <algorithm>
#include <cstring>
#include <vector>
namespace {
UnicodeString FromUTF8(const std::string& text)
{
    UnicodeString result;
    char* data = SDL_iconv_string("UTF-16LE", "UTF-8", text.c_str(), text.size() + 1);
    if (!data) return result;
    const auto* bytes = reinterpret_cast<const unsigned char*>(data);
    std::size_t units = 0;
    while (bytes[units * 2] || bytes[units * 2 + 1]) ++units;
    const auto wide = Platform::DecodeUTF16LE<WideChar>(bytes, units);
    result.set(wide.c_str());
    SDL_free(data);
    return result;
}
class SDLIMEManager final : public IMEManagerInterface {
    GameWindow* Window = nullptr;
    Int Disabled = 0, Displayed = 0, Cursor = 0, Selected = -1;
    Bool Ready = FALSE, Composing = FALSE;
    UnicodeString Composition;
    std::vector<UnicodeString> Candidates;
    static void Receive(const Platform::TextInputEvent& event)
    {
        if (TheIMEManager) static_cast<SDLIMEManager*>(TheIMEManager)->receive(event);
    }
    void clearComposition()
    {
        Composing = FALSE;
        while (Window && Displayed > 0) {
            TheWindowManager->winSendInputMsg(Window, GWM_CHAR, KEY_BACKSPACE, KEY_STATE_DOWN);
            --Displayed;
        }
        Displayed = 0;
        Composition.clear();
        Cursor = 0;
    }
    void insert(const UnicodeString& text)
    {
        for (Int i = 0; Window && i < text.getLength(); ++i)
            TheWindowManager->winSendInputMsg(Window, GWM_IME_CHAR, text.getCharAt(i), 0);
    }
    void receive(const Platform::TextInputEvent& event)
    {
        if (!Window || !isEnabled()) return;
        if (event.kind == Platform::TextInputKind::Return) {
            if (!Composing) TheWindowManager->winSendInputMsg(Window, GWM_IME_CHAR, '\r', 0);
        } else if (event.kind == Platform::TextInputKind::Candidates) {
            Candidates.clear();
            for (const auto& candidate : event.candidates) Candidates.push_back(FromUTF8(candidate));
            Selected = event.selected;
        } else {
            clearComposition();
            const auto text = FromUTF8(event.text);
            insert(text);
            if (event.kind == Platform::TextInputKind::Composition) {
                Composition = text;
                Displayed = text.getLength();
                Cursor = std::max(0, event.start);
                Composing = Displayed != 0;
            } else {
                Candidates.clear();
                Selected = -1;
            }
        }
    }
    void apply()
    {
        if (!Platform::EnableTextInput(Window && Ready && Disabled == 0))
            DEBUG_LOG(("SDL text input failed\n"));
    }
public:
    ~SDLIMEManager() override { detatch(); Platform::SetTextInputHandler(nullptr); }
    void init() override { Ready = Platform::HasGameWindow(); Platform::SetTextInputHandler(Receive); apply(); }
    void reset() override { clearComposition(); Candidates.clear(); Selected = -1; }
    void update() override {
        if (Window) {
            Int x,y,width,height,cursorX,cursorY;
            Window->winGetScreenPosition(&x,&y);
            Window->winGetSize(&width,&height);
            Window->winGetCursorPosition(&cursorX,&cursorY);
            Platform::SetTextInputArea(x,y,width,height,cursorX);
        }
    }
    void attach(GameWindow* window) override { clearComposition(); Window = window; apply(); update(); }
    void detatch() override { attach(nullptr); }
    void enable() override { if (--Disabled <= 0) Disabled = 0; apply(); }
    void disable() override { ++Disabled; apply(); }
    Bool isEnabled() override { return Ready && Disabled == 0; }
    Bool isAttachedTo(GameWindow* window) override { return Window == window; }
    GameWindow* getWindow() override { return Window; }
    Bool isComposing() override { return Composing; }
    void getCompositionString(UnicodeString& text) override { text = Composition; }
    Int getCompositionCursorPosition() override { return Cursor; }
    Int getIndexBase() override { return 1; }
    Int getCandidateCount() override { return static_cast<Int>(Candidates.size()); }
    UnicodeString* getCandidate(Int index) override {
        return index >= 0 && static_cast<std::size_t>(index) < Candidates.size()
            ? &Candidates[index] : &UnicodeString::TheEmptyString;
    }
    Int getSelectedCandidateIndex() override { return Selected; }
    Int getCandidatePageSize() override { return getCandidateCount(); }
    Int getCandidatePageStart() override { return 0; }
};
}
IMEManagerInterface* TheIMEManager = nullptr;
IMEManagerInterface* CreateIMEManagerInterface() { return NEW SDLIMEManager; }