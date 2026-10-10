/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

// Win32OSDisplay.cpp //////////////////////////////////////
// John McDonald, December 2002
////////////////////////////////////////////////////////////

#include "Common/OSDisplay.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "GameClient/GameText.h"
#include "Platform/Dialogs.h"
#include "Platform/UTF16.h"

OSDisplayButtonType OSDisplayWarningBox(AsciiString p, AsciiString m, UnsignedInt buttonFlags, UnsignedInt otherFlags)
{
    if (!TheGameText) return OSDBT_ERROR;
    UnicodeString prompt = TheGameText->fetch(p);
    UnicodeString message = TheGameText->fetch(m);
    const auto caption = Platform::UTF16ToUTF8(Platform::EncodeUTF16LE(prompt.str(), prompt.getLength()));
    const auto text = Platform::UTF16ToUTF8(Platform::EncodeUTF16LE(message.str(), message.getLength()));
    const auto buttons = BitTest(buttonFlags, OSDBT_CANCEL) ? Platform::DialogButtons::OKCancel : Platform::DialogButtons::OK;
    auto icon = Platform::DialogIcon::None;
    if (BitTest(otherFlags, OSDOF_EXCLAMATIONICON)) icon = Platform::DialogIcon::Warning;
    if (BitTest(otherFlags, OSDOF_INFORMATIONICON)) icon = Platform::DialogIcon::None;
    if (BitTest(otherFlags, OSDOF_ERRORICON) || BitTest(otherFlags, OSDOF_STOPICON)) icon = Platform::DialogIcon::Error;
    return Platform::ShowDialog(text.c_str(), caption.c_str(), buttons, icon, Platform::DialogResult::OK, false)
        == Platform::DialogResult::OK ? OSDBT_OK : OSDBT_CANCEL;
}