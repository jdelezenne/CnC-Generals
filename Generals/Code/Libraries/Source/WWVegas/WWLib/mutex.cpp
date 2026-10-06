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

#include "mutex.h"
#include "wwdebug.h"
#include "Platform/Synchronization.h"

MutexClass::MutexClass(const char* name) : handle(Platform::CreateRecursiveMutex(name)), locked(false)
{
	WWASSERT(handle);
}
MutexClass::~MutexClass()
{
	WWASSERT(!locked); // Can't delete locked mutex!
	Platform::DestroyRecursiveMutex(handle);
}
bool MutexClass::Lock(int time)
{
	if (!Platform::LockRecursiveMutex(handle, time)) return false;
	locked++;
	return true;
}
void MutexClass::Unlock()
{
	WWASSERT(locked);
	int res = Platform::UnlockRecursiveMutex(handle);
	res; // silence compiler warnings
	WWASSERT(res);
	locked--;
}
MutexClass::LockClass::LockClass(MutexClass& mutex_,int time) : mutex(mutex_)
{
	failed=!mutex.Lock(time);
}
MutexClass::LockClass::~LockClass()
{
	if (!failed) mutex.Unlock();
}

CriticalSectionClass::CriticalSectionClass() : handle(Platform::CreateCriticalSection()), locked(false)
{
}
CriticalSectionClass::~CriticalSectionClass()
{
	WWASSERT(!locked); // Can't delete locked mutex!
	Platform::DestroyCriticalSection(handle);
}
void CriticalSectionClass::Lock()
{
	Platform::EnterCriticalSection(handle);
	locked++;
}
void CriticalSectionClass::Unlock()
{
	WWASSERT(locked);
	Platform::LeaveCriticalSection(handle);
	locked--;
}
CriticalSectionClass::LockClass::LockClass(CriticalSectionClass& critical_section) : CriticalSection(critical_section)
{
	CriticalSection.Lock();
}
CriticalSectionClass::LockClass::~LockClass()
{
	CriticalSection.Unlock();
}
