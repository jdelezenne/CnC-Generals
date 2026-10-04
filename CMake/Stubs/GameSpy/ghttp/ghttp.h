#pragma once
#include "../../../../Vendors/GameSpy-2011/GameSpy/ghttp/ghttp.h"
typedef GHTTPBool (*GENHttpCompletedCallback)(GHTTPRequest, GHTTPResult, char *, int, void *);
inline GHTTPRequest ghttpGetA(const char *, GHTTPBool, GENHttpCompletedCallback, void *)
{ return -1; }
inline GHTTPRequest ghttpHeadA(const char *, GHTTPBool, GENHttpCompletedCallback, void *)
{ return -1; }
