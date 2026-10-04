#pragma once

#define WIN32_LEAN_AND_MEAN
#define AnimateWindow Win32AnimateWindow
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mmsystem.h>
#undef AnimateWindow
#ifdef __cplusplus
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>
#include <atlbase.h>
#include <atlcom.h>
#include <comutil.h>
#include <comdef.h>
#endif
// These SDK macros collide with names already used by the original game.
#undef BitTest
#undef AI_PASSIVE
#undef FILE_TYPE_UNKNOWN
// The SDK renamed this DWORD at the same FLOATING_SAVE_AREA offset.
#define Cr0NpxState Spare0

#ifdef __cplusplus
#include <new.h>
#include <vcruntime_exception.h>

// VCRuntime already supplies the placement operators declared by the game.
#define _OPERATOR_NEW_DEFINED_
void* __cdecl operator new(size_t, const char*, int);
void __cdecl operator delete(void*, const char*, int);
void* __cdecl operator new[](size_t, const char*, int);
void __cdecl operator delete[](void*, const char*, int);
#endif
