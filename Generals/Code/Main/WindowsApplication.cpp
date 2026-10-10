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

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

// FILE: WindowsApplication.cpp //////////////////////////////////////////////////////////
//
// Entry point for game application
//
// Author: Colin Day, April 2001
//
///////////////////////////////////////////////////////////////////////////////

#include "WinMain.h"
#include "WindowsApplication.h"
#include "Common/Debug.h"
#include "Common/StackDump.h"

#include <crtdbg.h>
#include <eh.h>

HINSTANCE ApplicationHInstance = NULL;
DWORD TheMessageTime = 0;

void InitializeNativeApplicationDiagnostics()
{
    _set_se_translator(DumpExceptionInfo);
}

void InitializeNativeWindowFeatures()
{
    ApplicationHInstance = GetModuleHandleA(NULL);
}

bool HandleNativeApplicationCommandLine(int argc, char* argv[])
{
		if (argc>2 && strcmp(argv[1],"-DX")==0) {
			Int i;
			DEBUG_LOG(("\n--- DX STACK DUMP\n"));
			for (i=2; i<argc; i++) {
				Int pc;
				pc = 0;
				sscanf(argv[i], "%x",  &pc);
				char name[_MAX_PATH], file[_MAX_PATH];
				unsigned int line;
				unsigned int addr;
				GetFunctionDetails((void*)pc, name, file, &line, &addr);
				DEBUG_LOG(("0x%x - %s, %s, line %d address 0x%x\n", pc, name, file, line, addr));
			}
			DEBUG_LOG(("\n--- END OF DX STACK DUMP\n"));
			return true;
		}

    return false;
}

void EnableNativeApplicationHeapTracking()
{
		#ifdef _DEBUG
			// Turn on Memory heap tracking
			int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
			tmpFlag |= (_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
			tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;
			_CrtSetDbgFlag( tmpFlag );
		#endif



}
