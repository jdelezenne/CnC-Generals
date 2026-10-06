#pragma once

#ifdef _WIN32
#include <objbase.h>
#include <windows.h>
using D3D8WindowHandle = HWND;
using D3D8MonitorHandle = HMONITOR;
using D3D8PaletteEntry = PALETTEENTRY;
#else
#include <wsl/winadapter.h>
using D3D8WindowHandle = void*;
using D3D8MonitorHandle = void*;
struct D3D8PaletteEntry { BYTE peRed, peGreen, peBlue, peFlags; };
struct RGNDATA;
#define DUMMYSTRUCTNAME
#define DUMMYUNIONNAME
#define DECLARE_INTERFACE_IID_(name, base, uuid) DECLARE_INTERFACE_(name, base)
#define MAKE_HRESULT(severity, facility, code) \
    static_cast<HRESULT>((static_cast<DWORD>(severity) << 31) | (static_cast<DWORD>(facility) << 16) | static_cast<DWORD>(code))
#ifndef WINAPI
#define WINAPI
#endif
#endif

#ifndef __MSABI_LONG
#define __MSABI_LONG(value) value##L
#endif

#ifndef MAKEFOURCC
#define MAKEFOURCC(a,b,c,d) (static_cast<DWORD>(a) | (static_cast<DWORD>(b)<<8) | (static_cast<DWORD>(c)<<16) | (static_cast<DWORD>(d)<<24))
#endif
