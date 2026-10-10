/*
**	Command & Conquer Generals(tm)
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

enum StealthLookType
{
	STEALTHLOOK_NONE,								///< unit is not stealthed at all
	STEALTHLOOK_VISIBLE_FRIENDLY,		///< unit is stealthed-but-visible due to friendly status
	STEALTHLOOK_DISGUISED_ENEMY,		///< we can have units that are disguised (instead of invisible)
	STEALTHLOOK_VISIBLE_DETECTED,		///< unit is stealthed and invisible, but a second material pass
																						///< is added to reveal the invisible unit as with heat vision
	STEALTHLOOK_VISIBLE_FRIENDLY_DETECTED,		///< unit is stealthed-but-visible due to being detected, 
																						///< and rendered in heatvision effect second material pass
	STEALTHLOOK_INVISIBLE						///< unit is stealthed-and-invisible
};
