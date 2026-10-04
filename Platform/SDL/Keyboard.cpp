// SPDX-License-Identifier: GPL-3.0-or-later
#include "Platform/SDL/Keyboard.h"
#include "Platform/Input.h"
void SDLKeyboard::init()
{
    Keyboard::init();
    if (getCapsState()) m_modifiers |= KEY_STATE_CAPSLOCK;
    else m_modifiers &= ~KEY_STATE_CAPSLOCK;
}
void SDLKeyboard::reset() { Platform::ResetKeyboardInput(); Keyboard::reset(); }
void SDLKeyboard::update() { m_eventsThisUpdate = 0; Keyboard::update(); }
Bool SDLKeyboard::getCapsState() { return Platform::CapsLockEnabled(); }
void SDLKeyboard::getKey(KeyboardIO* key)
{
    key->sequence = 0;
    key->key = KEY_NONE;
    // The game appends a repeat key and an end marker to its fixed-size buffer.
    if (m_eventsThisUpdate >= NUM_KEYS - 2) return;
    Platform::KeyEvent event;
    if (!Platform::ReadKeyEvent(event)) return;
    ++m_eventsThisUpdate;
    key->key = static_cast<UnsignedByte>(event.key);
    key->sequence = event.sequence;
    key->state = event.down ? KEY_STATE_DOWN : KEY_STATE_UP;
    key->status = KeyboardIO::STATUS_UNUSED;
}
