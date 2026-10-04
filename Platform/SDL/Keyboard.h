// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "GameClient/Keyboard.h"
class SDLKeyboard : public Keyboard {
public:
    virtual void init();
    virtual void reset();
    virtual void update();
    virtual Bool getCapsState();
protected:
    virtual void getKey(KeyboardIO* key);
private:
    UnsignedInt m_eventsThisUpdate = 0;
};
