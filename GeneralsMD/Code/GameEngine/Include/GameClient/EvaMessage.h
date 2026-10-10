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

enum EvaMessage
{
  EVA_Invalid = -1,
    
	EVA_FIRST = 0,
	EVA_LowPower = EVA_FIRST,
	EVA_InsufficientFunds,
	EVA_SuperweaponDetected_Own_ParticleCannon,
	EVA_SuperweaponDetected_Own_Nuke,
	EVA_SuperweaponDetected_Own_ScudStorm,
  EVA_SuperweaponDetected_Ally_ParticleCannon,
  EVA_SuperweaponDetected_Ally_Nuke,
  EVA_SuperweaponDetected_Ally_ScudStorm,
  EVA_SuperweaponDetected_Enemy_ParticleCannon,
  EVA_SuperweaponDetected_Enemy_Nuke,
  EVA_SuperweaponDetected_Enemy_ScudStorm,
	EVA_SuperweaponLaunched_Own_ParticleCannon,
	EVA_SuperweaponLaunched_Own_Nuke,
	EVA_SuperweaponLaunched_Own_ScudStorm,
  EVA_SuperweaponLaunched_Ally_ParticleCannon,
  EVA_SuperweaponLaunched_Ally_Nuke,
  EVA_SuperweaponLaunched_Ally_ScudStorm,
  EVA_SuperweaponLaunched_Enemy_ParticleCannon,
  EVA_SuperweaponLaunched_Enemy_Nuke,
  EVA_SuperweaponLaunched_Enemy_ScudStorm,
  EVA_SuperweaponReady_Own_ParticleCannon,
  EVA_SuperweaponReady_Own_Nuke,
  EVA_SuperweaponReady_Own_ScudStorm,
  EVA_SuperweaponReady_Ally_ParticleCannon,
  EVA_SuperweaponReady_Ally_Nuke,
  EVA_SuperweaponReady_Ally_ScudStorm,
  EVA_SuperweaponReady_Enemy_ParticleCannon,
  EVA_SuperweaponReady_Enemy_Nuke,
  EVA_SuperweaponReady_Enemy_ScudStorm,
	EVA_BuldingLost,
	EVA_BaseUnderAttack,
	EVA_AllyUnderAttack,
	EVA_BeaconDetected,
  EVA_EnemyBlackLotusDetected,
  EVA_EnemyJarmenKellDetected,
  EVA_EnemyColonelBurtonDetected,
  EVA_OwnBlackLotusDetected,
  EVA_OwnJarmenKellDetected,
  EVA_OwnColonelBurtonDetected,
	EVA_UnitLost,
	EVA_GeneralLevelUp,
	EVA_VehicleStolen,
	EVA_BuildingStolen,
	EVA_CashStolen,
	EVA_UpgradeComplete,
	EVA_BuildingBeingStolen,
	EVA_BuildingSabotaged,
	EVA_SuperweaponLaunched_Own_GPS_Scrambler,
  EVA_SuperweaponLaunched_Ally_GPS_Scrambler,
  EVA_SuperweaponLaunched_Enemy_GPS_Scrambler,
	EVA_SuperweaponLaunched_Own_Sneak_Attack,
  EVA_SuperweaponLaunched_Ally_Sneak_Attack,
  EVA_SuperweaponLaunched_Enemy_Sneak_Attack,

	EVA_COUNT,
};
