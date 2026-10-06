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

enum DamageType
{	
	DAMAGE_EXPLOSION							= 0,			
	DAMAGE_CRUSH									= 1,					
	DAMAGE_ARMOR_PIERCING					= 2,
	DAMAGE_SMALL_ARMS							= 3,		
	DAMAGE_GATTLING								= 4,				
	DAMAGE_RADIATION							= 5,			
	DAMAGE_FLAME									= 6,					
	DAMAGE_LASER									= 7,					
	DAMAGE_SNIPER									= 8,				
	DAMAGE_POISON									= 9,			
	DAMAGE_HEALING								= 10,	
	DAMAGE_UNRESISTABLE						= 11,		// this is for scripting to cause 'armorproof' damage
	DAMAGE_WATER									= 12,	
	DAMAGE_DEPLOY									= 13,					// for transports to deploy units and order them to all attack.
// this stays, even if ALLOW_SURRENDER is not defed, since flashbangs still use 'em
	DAMAGE_SURRENDER							= 14,				// if something "dies" to surrender damage, they surrender.... duh!
	DAMAGE_HACK										= 15,
	DAMAGE_KILLPILOT							= 16,				// special snipe attack that kills the pilot and renders a vehicle unmanned.
	DAMAGE_PENALTY								= 17,					// from game penalty (you won't receive radar warnings BTW)
	DAMAGE_FALLING								= 18,
	DAMAGE_MELEE									= 19,						// Blades, clubs...
	DAMAGE_DISARM									= 20,	// "special" damage type used for disarming mines, bombs, etc (NOT for "disarming" an opponent!)
	DAMAGE_HAZARD_CLEANUP					= 21,	// special damage type for cleaning up hazards like radiation or bio-poison.
	DAMAGE_PARTICLE_BEAM					= 22,	// Incinerates virtually everything (insanely powerful orbital beam)
	DAMAGE_TOPPLING								= 23,	// damage from getting toppled.
	DAMAGE_INFANTRY_MISSILE				= 24,	
	DAMAGE_AURORA_BOMB						= 25,	
	DAMAGE_LAND_MINE							= 26,	
	DAMAGE_JET_MISSILES						= 27,	
	DAMAGE_STEALTHJET_MISSILES		= 28,	
	DAMAGE_MOLOTOV_COCKTAIL				= 29,	
	DAMAGE_COMANCHE_VULCAN				= 30,	
	DAMAGE_FLESHY_SNIPER					= 31,		// like DAMAGE_SNIPER, but (generally) does no damage to vehicles.

	// Please note: There is a string array below this enum, and when you change them,
	// you need to search on the array names to find all the stuff that generates names
	// based on these strings.  (eg DamageFX does a strcat to make its array of names so 
	// change DamageFX.ini and its Default)


	// !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
	// if you add additional damage types, you will PROBABLY HAVE TO
	// ENLARGE A BITMASK IN WEAPONSET.
	//
	// !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	DAMAGE_NUM_TYPES			// keep this last
};
