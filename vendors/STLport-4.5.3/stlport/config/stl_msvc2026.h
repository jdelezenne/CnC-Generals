// Modified for the MSVC 2026 CRT header layout and compiler capabilities.
#include <gen_stlport_native.h>

#define _STLP_CALL __cdecl
#define _STLP_LONG_LONG __int64
#define _STLP_WCHAR_T_IS_USHORT 1
#define _STLP_USE_PRAGMA_ONCE 1
#define _STLP_HAS_NO_NEW_C_HEADERS 1
#define _STLP_VENDOR_GLOBAL_CSTD 1
#define _STLP_NO_MSVC50_COMPATIBILITY 1
#define _STLP_NO_USING_FOR_GLOBAL_FUNCTIONS 1
#define _STLP_LABS labs
#define _STLP_LDIV ldiv

#ifndef _CPPUNWIND
#define _STLP_HAS_NO_EXCEPTIONS 1
#endif
#if defined(_MT) && !defined(_STLP_NO_THREADS) && !defined(_REENTRANT)
#define _REENTRANT 1
#endif
