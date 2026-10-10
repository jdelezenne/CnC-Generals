#pragma once
#include "../PlatformBegin.h"
#include "../../../../Vendors/GameSpy-2011/GameSpy/Peer/peer.h"
#include "../PlatformEnd.h"
typedef SBServer GServer;
typedef void (*GENPeerNickErrorCallback)(PEER, int, const char *, void *);
typedef void (*GENPeerConnectCallback)(PEER, PEERBool, void *);
inline void chatSetLocalIP(unsigned int) {}
inline void peerConnect(PEER peer, const char *nick, int profileID,
    GENPeerNickErrorCallback nickError, GENPeerConnectCallback callback,
    void *param, PEERBool blocking)
{
    if (callback)
        callback(peer, PEERFalse, param);
}
