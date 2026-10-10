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

#include "Registry.h"
#include "Platform/Settings.h"

bool GetStringFromRegistry(std::string path, std::string key, std::string& val)
{
    char buffer[256];
    if (!Platform::ReadInstallationString(Platform::CurrentGame(), path.c_str(), key.c_str(), buffer, sizeof(buffer)))
        return false;
    val = buffer;
    return true;
}

bool GetUnsignedIntFromRegistry(std::string path, std::string key, unsigned int& val)
{
    return Platform::ReadInstallationUnsigned(Platform::CurrentGame(), path.c_str(), key.c_str(), val);
}

bool SetStringInRegistry(std::string path, std::string key, std::string val)
{
    return Platform::WriteInstallationString(Platform::CurrentGame(), path.c_str(), key.c_str(), val.c_str());
}

bool SetUnsignedIntInRegistry(std::string path, std::string key, unsigned int val)
{
    return Platform::WriteInstallationUnsigned(Platform::CurrentGame(), path.c_str(), key.c_str(), val);
}
