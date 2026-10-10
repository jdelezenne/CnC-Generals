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

enum WeaponChoiceCriteria
{
	PREFER_MOST_DAMAGE,		///< choose the weapon that will do the most damage
	PREFER_LONGEST_RANGE	///< choose the weapon with the longest range (that will do nonzero damage)
};

enum WeaponLockType
{
	NOT_LOCKED,							///< Weapon is not locked
	LOCKED_TEMPORARILY,			///< Weapon is locked until clip is empty, or current "attack" state exits
	LOCKED_PERMANENTLY			///< Weapon is locked until explicitly unlocked or lock is changed to another weapon
};

enum CanAttackResult
{
	//Worst scenario to best scenario -- These must be done this way now!
	ATTACKRESULT_NOT_POSSIBLE,					//Can't possibly attack target.
	ATTACKRESULT_INVALID_SHOT,					//Not a clear shot
	ATTACKRESULT_POSSIBLE_AFTER_MOVING, //I can attack, but after moving closer.
	ATTACKRESULT_POSSIBLE,							//I can attack now.
};
