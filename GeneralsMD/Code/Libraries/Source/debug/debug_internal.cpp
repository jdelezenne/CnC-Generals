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

/////////////////////////////////////////////////////////////////////////EA-V1
// $File: //depot/GeneralsMD/Staging/code/Libraries/Source/debug/debug_internal.cpp $
// $Author: mhoffe $
// $Revision: #1 $
// $DateTime: 2003/07/03 11:55:26 $
//
// ©2003 Electronic Arts
//
// Implementation of internal code
//////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include "Platform/Memory.h"
#include "_pch.h"

void DebugInternalAssert(const char *file, int line, const char *expr)
{
  // dangerous as well but since this function is used in this
  // module only we know how long stuff can get
  char buf[512];
  sprintf(buf,"File %s, line %i:\n%s",file,line,expr);
  Platform::ShowDialog(buf,"Internal assert failed",Platform::DialogButtons::OK,Platform::DialogIcon::Error,Platform::DialogResult::OK,true);
  
  // stop right now!
  std::_Exit(666);
}

void *DebugAllocMemory(unsigned numBytes)
{
  void* memory = Platform::AllocateSystemMemory(numBytes, false);
  if (!memory) DCRASH_RELEASE("Diagnostic memory allocation failed");
  return memory;
}

void *DebugReAllocMemory(void *oldPtr, unsigned newSize)
{
  if (!newSize) { Platform::FreeSystemMemory(oldPtr); return NULL; }
  void* memory = DebugAllocMemory(newSize);
  if (oldPtr) memcpy(memory, oldPtr, std::min<std::size_t>(Platform::SystemMemorySize(oldPtr), newSize));
  Platform::FreeSystemMemory(oldPtr);
  return memory;
}

void DebugFreeMemory(void *ptr)
{
  if (ptr)
    Platform::FreeSystemMemory(ptr);
}
