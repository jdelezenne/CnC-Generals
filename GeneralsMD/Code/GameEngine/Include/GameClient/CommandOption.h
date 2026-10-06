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

enum CommandOption
{
	COMMAND_OPTION_NONE					= 0x00000000,
	NEED_TARGET_ENEMY_OBJECT		= 0x00000001, // command now needs user to select enemy target
	NEED_TARGET_NEUTRAL_OBJECT	= 0x00000002, // command now needs user to select neutral target
	NEED_TARGET_ALLY_OBJECT			= 0x00000004, // command now needs user to select ally target
#ifdef ALLOW_SURRENDER
	NEED_TARGET_PRISONER				= 0x00000008, // needs user to now select prisoner object
#endif
	ALLOW_SHRUBBERY_TARGET			= 0x00000010, // allow neutral shrubbery as a target
	NEED_TARGET_POS							= 0x00000020, // command now needs user to select target position
	NEED_UPGRADE								= 0x00000040, // command requires upgrade to be enabled
	NEED_SPECIAL_POWER_SCIENCE	= 0x00000080, // command requires a science in the special power specified
	OK_FOR_MULTI_SELECT					= 0x00000100, // command is ok to show when multiple objects selected
	CONTEXTMODE_COMMAND					= 0x00000200, // a context sensitive command mode that requires code to determine whether cursor is valid or not.
	CHECK_LIKE									= 0x00000400, // dynamically change the UI element push button to be "check like"
	ALLOW_MINE_TARGET						= 0x00000800, // allow (land)mines as a target
	ATTACK_OBJECTS_POSITION			=	0x00001000, // for weapons that need an object target but attack the position indirectly (like burning trees)
	OPTION_ONE									= 0x00002000, // User data -- option 1
	OPTION_TWO									= 0x00004000,	// User data -- option 2
	OPTION_THREE								= 0x00008000,	// User data -- option 3
	NOT_QUEUEABLE								= 0x00010000,	// Option not build queueable meaning you can only build it when queue is empty!
	SINGLE_USE_COMMAND					= 0x00020000, // Once used, it can never be used again!
	COMMAND_FIRED_BY_SCRIPT			= 0x00040000, // Used only by code to tell special powers that they have been fired by a script.
	SCRIPT_ONLY									= 0x00080000, // Only a script can use this command (not by users)
	IGNORES_UNDERPOWERED				= 0x00100000, // this button isn't disabled if its object is merely underpowered
	USES_MINE_CLEARING_WEAPONSET= 0x00200000,	// uses the special mine-clearing weaponset, even if not current
	CAN_USE_WAYPOINTS						= 0x00400000, // button has option to use a waypoint path
	MUST_BE_STOPPED							= 0x00800000, // Unit must be stopped in order to be able to use button.

	NUM_COMMAND_OPTIONS						// keep this last
};
