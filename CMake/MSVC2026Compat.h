#pragma once

#ifdef __cplusplus
// Load CRT/ATL support before STLport redirects std:: to its own namespace.
#define _STLP_DONT_REDEFINE_STD
#endif
#define WIN32_LEAN_AND_MEAN
#define AnimateWindow Win32AnimateWindow
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mmsystem.h>
#undef AnimateWindow
#define _STLP_WINDOWS_H_INCLUDED
#ifdef __cplusplus
#include <gen_native_cpp/cstddef>
#include <gen_native_cpp/cstdlib>
#include <gen_native_cpp/cstring>
#include <gen_native_cpp/new>
#include <gen_native_cpp/utility>
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
#undef _STLP_DONT_REDEFINE_STD
#define std STLPORT
#endif
