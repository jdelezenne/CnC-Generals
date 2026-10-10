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

// FILE: ThreadUtils.cpp //////////////////////////////////////////////////////
// GameSpy thread utils
// Author: Matthew D. Campbell, July 2002

#include "PreRTS.h"
#include "Platform/UTF16.h"
#include "Platform/Dialogs.h"
#include <SDL3/SDL_stdinc.h>	// This must go first in EVERY cpp file int the GameEngine

//-------------------------------------------------------------------------

std::wstring MultiByteToWideCharSingleLine(const char* orig)
{
    char* encoded = SDL_iconv_string("UTF-16LE", "UTF-8", orig, strlen(orig) + 1);
    if (!encoded) return {};
    const auto* bytes = reinterpret_cast<const unsigned char*>(encoded);
    std::size_t count = 0;
    while (bytes[count * 2] || bytes[count * 2 + 1]) ++count;
    auto result = Platform::DecodeUTF16LE<WideChar>(bytes, count);
    SDL_free(encoded);
    for (auto& c : result) if (c == L'\n' || c == L'\r') c = L' ';
    return result;
}

std::string WideCharStringToMultiByte(const WideChar* orig)
{
    return Platform::UTF16ToUTF8(Platform::EncodeUTF16LE(orig, wcslen(orig)));
}
