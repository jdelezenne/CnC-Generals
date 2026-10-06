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

enum RadarPriorityType
{
	RADAR_PRIORITY_INVALID,					// a priority that has not been set (in general it won't show up on the radar)
	RADAR_PRIORITY_NOT_ON_RADAR,		// object specifically forbidden from being on the radar
	RADAR_PRIORITY_STRUCTURE,				// structure level drawing priority
	RADAR_PRIORITY_UNIT,						// unit level drawing priority
	RADAR_PRIORITY_LOCAL_UNIT_ONLY,	// unit priority, but only on the radar if controlled by the local player

	RADAR_PRIORITY_NUM_PRIORITIES		// keep this last
};
