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

////////////////////////////////////////////////////////////////////////////////
//																																						//
//  (c) 2001-2003 Electronic Arts Inc.																				//
//																																						//
////////////////////////////////////////////////////////////////////////////////

// FILE: Application.cpp //////////////////////////////////////////////////////////
//
// Entry point for game application
//
// Author: Colin Day, April 2001
//
///////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_main.h>
#include "Platform/Application.h"
#include "Platform/Paths.h"
#include "Platform/Window.h"
#include "Lib/BaseType.h"
#include "Common/CriticalSection.h"
#include "Common/GlobalData.h"
#include "Common/GameEngine.h"
#include "Common/GameSounds.h"
#include "Common/Debug.h"
#include "Common/GameMemory.h"
#include "Common/MessageStream.h"
#include "Common/version.h"
#include "GameClient/Keyboard.h"
#include "GameClient/Mouse.h"
#include "Win32Device/GameClient/Win32Mouse.h"
#include "BuildVersion.h"
#include "GeneratedVersion.h"
#ifdef _WIN32
#include "WindowsApplication.h"
#endif
#include "Common/Registry.h"
#include <rts/profile.h>
#include <rts/debug.h>

Bool ApplicationIsWindowed = false;
Win32Mouse* TheWin32Mouse = NULL;
const Char* g_strFile = "data\\Generals.str";
const Char* g_csfFile = "data\\%s\\Generals.csf";
char* gAppPrefix = "";
#define GENERALS_GUID "685EAFF2-3216-4265-B047-251C5F4B82F3"
extern void Reset_D3D_Device(bool active);

static void onGameWindowEvent(Platform::WindowEvent event)
{
    if (event == Platform::WindowEvent::Close) {
        if (TheGameEngine && !TheGameEngine->getQuitting() && TheMessageStream)
            TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_INSTANT_QUIT);
        return;
    }
    const Bool active = event == Platform::WindowEvent::FocusGained;
    if (TheKeyboard) TheKeyboard->resetKeys();
    if (TheWin32Mouse) TheWin32Mouse->lostFocus(!active);
    if (TheGameEngine) {
        TheGameEngine->setIsActive(active);
        Reset_D3D_Device(active);
    }
    if (active && TheWin32Mouse) TheWin32Mouse->setCursor(TheWin32Mouse->getMouseCursor());
    if (TheAudio) {
        if (active) TheAudio->regainFocus();
        else TheAudio->loseFocus();
    }
}

static Bool initializeAppWindow(Bool windowed)
{
    Platform::SetWindowEventHandler(onGameWindowEvent);
    if (!Platform::CreateGameWindow("Command and Conquer Generals", 800, 600, windowed))
        return FALSE;
#ifdef _WIN32
    InitializeNativeWindowFeatures();
#endif
    return TRUE;
}

struct ApplicationInstanceLifetime
{
    void* handle = nullptr;
    ~ApplicationInstanceLifetime() { Platform::ReleaseApplicationInstance(handle); }
};

// Necessary to allow memory managers and such to have useful critical sections.
static CriticalSection critSec1, critSec2, critSec3, critSec4, critSec5;

struct ApplicationCriticalSectionLifetime
{
    ApplicationCriticalSectionLifetime() { std::atexit(Release); }
    ~ApplicationCriticalSectionLifetime() { Release(); }
    static void Release()
    {
        TheUnicodeStringCriticalSection = NULL;
        TheDmaCriticalSection = NULL;
        TheMemoryPoolCriticalSection = NULL;
        TheDebugLogCriticalSection = NULL;
    }
};

int main(int argc, char* argv[])
{
    Debug::Initialize();
    ApplicationInstanceLifetime instance;
    ApplicationCriticalSectionLifetime criticalSections;

#ifdef _PROFILE
  Profile::StartRange("init");
#endif

	try {

#ifdef _WIN32
        InitializeNativeApplicationDiagnostics();
#endif
		TheUnicodeStringCriticalSection = &critSec2;
		TheDmaCriticalSection = &critSec3;
		TheMemoryPoolCriticalSection = &critSec4;
		TheDebugLogCriticalSection = &critSec5;

        const Int argumentCount = argc < 20 ? argc : 20;
        char* gameArguments[20];
        gameArguments[0] = NULL;
        for (Int i = 1; i < argumentCount; ++i) {
            gameArguments[i] = argv[i];
            if (SDL_strcasecmp(argv[i], "-win") == 0) ApplicationIsWindowed = true;
        }
        argc = argumentCount;
        argv = gameArguments;
#ifdef _WIN32
        if (HandleNativeApplicationCommandLine(argc, argv)) return 0;
        EnableNativeApplicationHeapTracking();
#endif
		// install debug callbacks
	//	WWDebug_Install_Message_Handler(WWDebug_Message_Callback);
	//	WWDebug_Install_Assert_Handler(WWAssert_Callback);


		if (initializeAppWindow(ApplicationIsWindowed) == false)
			return 0;

// Force "splash image" to be loaded from a file, not a resource so same exe can be used in different localizations.
#if defined _DEBUG || defined _INTERNAL || defined _PROFILE

			// check both localized directory and root dir
		char filePath[260];
		char *fileName = "Install_Final.bmp";
		static const char *localizedPathFormat = "Data/%s/";
		sprintf(filePath,localizedPathFormat, GetRegistryLanguage().str());
		strcat( filePath, fileName );
		FILE *fileImage = Platform::OpenStream(filePath, "r");
		if (fileImage) {
			fclose(fileImage);
			Platform::ShowStartupSplash(filePath);
		}
		else {
			Platform::ShowStartupSplash(fileName);
		}
#else

		// in release, the file only ever lives in the root dir
		Platform::ShowStartupSplash("Install_Final.bmp");
#endif



		// BGC - initialize COM
	//	OleInitialize(NULL);

		// start the log
		DEBUG_INIT(DEBUG_FLAGS_DEFAULT);
		initMemoryManager();


		// Set up version info
		TheVersion = NEW Version;
		TheVersion->setVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNUM, VERSION_LOCALBUILDNUM,
			AsciiString(VERSION_BUILDUSER), AsciiString(VERSION_BUILDLOC),
			AsciiString(__TIME__), AsciiString(__DATE__));



        // Both games retain the original shared single-instance identifier.
        if (Platform::AcquireApplicationInstance(GENERALS_GUID, instance.handle) ==
            Platform::ApplicationInstanceResult::AlreadyRunning)
        {
			DEBUG_LOG(("Generals is already running...Bail!\n"));
			delete TheVersion;
			TheVersion = NULL;
			shutdownMemoryManager();
			DEBUG_SHUTDOWN();
			return 0;
		}
		DEBUG_LOG(("Create GeneralsMutex okay.\n"));


		DEBUG_LOG(("CRC message is %d\n", GameMessage::MSG_LOGIC_CRC));

		// run the game main loop
		GameMain(argc, argv);
		Platform::DestroyGameWindow();


		delete TheVersion;
		TheVersion = NULL;

	#ifdef MEMORYPOOL_DEBUG
		TheMemoryPoolFactory->debugMemoryReport(REPORT_POOLINFO | REPORT_POOL_OVERFLOW | REPORT_SIMPLE_LEAKS, 0, 0);
	#endif
	#if defined(_DEBUG) || defined(_INTERNAL)
		TheMemoryPoolFactory->memoryPoolUsageReport("AAAMemStats");
	#endif

		// close the log
		shutdownMemoryManager();
		DEBUG_SHUTDOWN();

		// BGC - shut down COM
	//	OleUninitialize();
	}
	catch (...)
	{

	}

	TheUnicodeStringCriticalSection = NULL;
	TheDmaCriticalSection = NULL;
	TheMemoryPoolCriticalSection = NULL;

	return 0;

}  // end main

