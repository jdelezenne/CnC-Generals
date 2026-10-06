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

#pragma once

enum AudioType
{
	AT_Music,
	AT_Streaming,
	AT_SoundEffect
};

enum AudioPriority
{
	AP_LOWEST,
	AP_LOW,
	AP_NORMAL,
	AP_HIGH,
	AP_CRITICAL
};

enum SoundType
{
	ST_UI										= 0x0001,
	ST_WORLD								= 0x0002,
	ST_SHROUDED							= 0x0004,
	ST_GLOBAL								= 0x0008,
	ST_VOICE								= 0x0010,
	ST_PLAYER								= 0x0020,
	ST_ALLIES								= 0x0040,
	ST_ENEMIES							= 0x0080,
	ST_EVERYONE							= 0x0100,	
};

enum AudioControl
{
	AC_LOOP									= 0x0001,
	AC_RANDOM								= 0x0002,
	AC_ALL									= 0x0004,
	AC_POSTDELAY						= 0x0008,
	AC_INTERRUPT						= 0x0010,
};
