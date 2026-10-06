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

enum TerrainDecalType
{
#ifdef ALLOW_DEMORALIZE
	TERRAIN_DECAL_DEMORALIZED = 0,
#else
	TERRAIN_DECAL_DEMORALIZED_OBSOLETE = 0,
#endif
	TERRAIN_DECAL_HORDE,
	TERRAIN_DECAL_HORDE_WITH_NATIONALISM_UPGRADE,
	TERRAIN_DECAL_HORDE_VEHICLE,
	TERRAIN_DECAL_HORDE_WITH_NATIONALISM_UPGRADE_VEHICLE,
	TERRAIN_DECAL_CRATE,
	TERRAIN_DECAL_NONE,

	TERRAIN_DECAL_MAX	///< keep this last
};
