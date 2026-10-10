#pragma once
#include "../PlatformBegin.h"
#include "../../../../Vendors/GameSpy-2011/GameSpy/ghttp/ghttp.h"
#include "../PlatformEnd.h"
#include <type_traits>
typedef GHTTPBool (*GENHttpCompletedCallback)(GHTTPRequest, GHTTPResult, char *, int, void *);
template<class Callback, std::enable_if_t<std::is_same_v<Callback, GENHttpCompletedCallback> &&
    !std::is_same_v<Callback, ghttpCompletedCallback>, int> = 0>
inline GHTTPRequest ghttpGetA(const char *, GHTTPBool, Callback, void *)
{ return -1; }
template<class Callback, std::enable_if_t<std::is_same_v<Callback, GENHttpCompletedCallback> &&
    !std::is_same_v<Callback, ghttpCompletedCallback>, int> = 0>
inline GHTTPRequest ghttpHeadA(const char *, GHTTPBool, Callback, void *)
{ return -1; }
