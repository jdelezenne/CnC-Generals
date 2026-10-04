// Offline API implementation. No SDK networking code is linked into this build.
#include "GameSpy/Peer/Peer.h"
#include "GameSpy/GP/GP.h"
#include "GameSpy/ghttp/ghttp.h"
#include "GameSpy/gstats/gstats.h"
#include "GameSpy/gstats/gpersist.h"
#include "GameSpy/pt/pt.h"
char gcd_secret_key[256] = {0};
char gcd_gamename[256] = {0};
char StatsServerHostname[64] = {0};
const char *qr2_registered_key_list[256] = {0};
extern "C" int getQR2HostingStatus(void) { return 0; }
#include "GameSpyFunctions.inc"
