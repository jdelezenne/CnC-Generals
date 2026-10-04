#pragma once
#include "../../../../Vendors/GameSpy-2011/GameSpy/GP/gp.h"
// The original application predates the namespace and partner arguments.
inline GPResult gpInitialize(GPConnection *connection, int productID)
{
    return gpInitialize(connection, productID, 0, 0);
}
inline GPResult gpConnectNewUser(GPConnection *connection, const char *nick,
    const char *email, const char *password, GPEnum firewall, GPEnum blocking,
    GPCallback callback, void *param)
{
    return GP_NETWORK_ERROR;
}
inline GPResult gpDeleteProfile(GPConnection *connection)
{
    return GP_NETWORK_ERROR;
}
