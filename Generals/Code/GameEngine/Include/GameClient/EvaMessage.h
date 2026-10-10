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

enum EvaMessage
{
	EVA_FIRST = 0,
	EVA_LowPower = EVA_FIRST,
	EVA_InsufficientFunds,
	EVA_SuperweaponDetected_ParticleCannon,
	EVA_SuperweaponDetected_Nuke,
	EVA_SuperweaponDetected_ScudStorm,
	EVA_SuperweaponLaunched_ParticleCannon,
	EVA_SuperweaponLaunched_Nuke,
	EVA_SuperweaponLaunched_ScudStorm,
	EVA_BuldingLost,
	EVA_BaseUnderAttack,
	EVA_AllyUnderAttack,
	EVA_BeaconDetected,
	EVA_UnitLost,
	EVA_GeneralLevelUp,
	EVA_VehicleStolen,
	EVA_BuildingStolen,
	EVA_CashStolen,
	EVA_UpgradeComplete,
	EVA_BuildingBeingStolen,

	EVA_COUNT,
};
