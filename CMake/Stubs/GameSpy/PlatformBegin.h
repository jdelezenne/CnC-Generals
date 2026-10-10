// The SDK's documented platform selectors belong only to SDK imports.
// This header is intentionally repeatable, paired with PlatformEnd.h.
#pragma push_macro("_UNIX")
#pragma push_macro("_LINUX")
#pragma push_macro("_MACOSX")
#if defined(__linux__) && !defined(_LINUX)
#define _LINUX
#elif defined(__APPLE__) && !defined(_MACOSX)
#define _MACOSX
#endif
