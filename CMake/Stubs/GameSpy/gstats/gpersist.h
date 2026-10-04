#pragma once
#include "../../../../Vendors/GameSpy-2011/GameSpy/gstats/gpersist.h"
typedef void (*GENPersistGetCallback)(int, int, persisttype_t, int, int, char *, int, void *);
typedef void (*GENPersistSetCallback)(int, int, persisttype_t, int, int, void *);
inline void GetPersistDataValuesA(int localid, int profileid, persisttype_t type,
    int index, const char *keys, GENPersistGetCallback, void *) {}
inline void SetPersistDataValuesA(int localid, int profileid, persisttype_t type,
    int index, const char *keys, GENPersistSetCallback, void *) {}
