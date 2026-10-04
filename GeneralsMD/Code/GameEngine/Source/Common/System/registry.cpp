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

// Registry.cpp
// Simple interface for storing/retreiving registry values
// Author: Matthew D. Campbell, December 2001

#include "PreRTS.h"	// This must go first in EVERY cpp file int the GameEngine

#include "Common/Registry.h"
#include "Platform/Settings.h"

#ifdef _INTERNAL
// for occasional debugging...
//#pragma optimize("", off)
//#pragma MESSAGE("************************************** WARNING, optimization disabled for debugging purposes")
#endif

Bool GetStringFromRegistry(AsciiString path, AsciiString key, AsciiString& val)
{
    char buffer[256];
    if (!Platform::ReadInstallationString(Platform::GameTitle::ZeroHour, path.str(), key.str(), buffer, sizeof(buffer)))
        return FALSE;
    val = buffer;
    return TRUE;
}

Bool GetUnsignedIntFromRegistry(AsciiString path, AsciiString key, UnsignedInt& val)
{
    return Platform::ReadInstallationUnsigned(Platform::GameTitle::ZeroHour, path.str(), key.str(), val);
}
Bool GetStringFromGeneralsRegistry(AsciiString path, AsciiString key, AsciiString& val)
{
    char buffer[256];
    if (!Platform::ReadInstallationString(Platform::GameTitle::Generals, path.str(), key.str(), buffer, sizeof(buffer)))
        return FALSE;
    val = buffer;
    return TRUE;
}

AsciiString GetRegistryLanguage(void)
{
	static Bool cached = FALSE;
	// NOTE: static causes a memory leak, but we have to keep it because the value is cached.
	static AsciiString val = "english";
	if (cached) {
		return val;
	} else {
		cached = TRUE;
	}

	GetStringFromRegistry("", "Language", val);
	return val;
}

AsciiString GetRegistryGameName(void)
{
	AsciiString val = "GeneralsMPTest";
	GetStringFromRegistry("", "SKU", val);
	return val;
}

UnsignedInt GetRegistryVersion(void)
{
	UnsignedInt val = 65536;
	GetUnsignedIntFromRegistry("", "Version", val);
	return val;
}

UnsignedInt GetRegistryMapPackVersion(void)
{
	UnsignedInt val = 65536;
	GetUnsignedIntFromRegistry("", "MapPackVersion", val);
	return val;
}
