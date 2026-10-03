// Generated offline definitions. No networking implementation is linked.
// Regenerate with cmake/stubs/generate-offline.py.
extern "C" {
void * BucketAdd(bucketset_t set, char *name, void *value) { return NULL; }
void * BucketAvg(bucketset_t set, char *name, void *value) { return NULL; }
void * BucketConcat(bucketset_t set, char *name, void *value) { return NULL; }
void * BucketDiv(bucketset_t set, char *name, void *value) { return NULL; }
void * BucketGet(bucketset_t set, char *name) { return NULL; }
void * BucketMult(bucketset_t set, char *name, void *value) { return NULL; }
void * BucketNew(bucketset_t set, char *name, BucketType type, void *initialvalue) { return NULL; }
void * BucketSet(bucketset_t set, char *name,void *value) { return NULL; }
void * BucketSub(bucketset_t set, char *name, void *value) { return NULL; }
void CloseStatsConnection(void) {  }
void FreeGame(statsgame_t game) {  }
char * GenerateAuthA(const char *challenge, const char *password,char response[33]) { return NULL; }
char * GetChallenge(statsgame_t game) { return NULL; }
void GetPersistData(int localid, int profileid, persisttype_t type, int index, PersDataCallbackFn callback, void *instance) {  }
void GetPersistDataModified(int localid, int profileid, persisttype_t type, int index, time_t modifiedsince, PersDataCallbackFn callback, void *instance) {  }
void GetPersistDataValuesA(int localid, int profileid, persisttype_t type, int index, char *keys, PersDataCallbackFn callback, void *instance) {  }
void GetPersistDataValuesModifiedA(int localid, int profileid, persisttype_t type, int index, time_t modifiedsince, char *keys, PersDataCallbackFn callback, void *instance) {  }
int InitStatsAsync(int gameport, gsi_time theInitTimeout) { return GE_NOCONNECT; }
int InitStatsConnection(int gameport) { return GE_NOCONNECT; }
int InitStatsThink() { return GE_NOCONNECT; }
int IsStatsConnected() { return (int)0; }
statsgame_t NewGame(int usebuckets) { return NULL; }
int PersistThink() { return (int)0; }
void PreAuthenticatePlayerCDA(int localid, const char *nick, const char *keyhash, const char *challengeresponse, PersAuthCallbackFn callback, void *instance) {  }
void PreAuthenticatePlayerPM(int localid, int profileid, const char *challengeresponse, PersAuthCallbackFn callback, void *instance) {  }
void PreAuthenticatePlayerPartner(int localid, const char* authtoken, const char *challengeresponse, PersAuthCallbackFn callback, void *instance) {  }
void SBServerAddIntKeyValue(SBServer server, const char *keyname, int value) {  }
void SBServerAddKeyValue(SBServer server, const char *keyname, const char *value) {  }
SBBool SBServerDirectConnect(SBServer server) { return (SBBool)0; }
void SBServerEnumKeys(SBServer server, SBServerKeyEnumFn KeyFn, void *instance) {  }
void SBServerFree(void *elem) {  }
SBBool SBServerGetBoolValueA(SBServer server, const char *key, SBBool bdefault) { return (SBBool)0; }
SBBool SBServerGetConnectionInfo(ServerBrowser gSB, SBServer server, gsi_u16 PortToConnectTo, char *ipstring_out) { return (SBBool)0; }
unsigned char SBServerGetFlags(SBServer server) { return (unsigned char)0; }
double SBServerGetFloatValueA(SBServer server, const char *key, double fdefault) { return fdefault; }
int SBServerGetIntValueA(SBServer server, const char *key, int idefault) { return idefault; }
SBServer SBServerGetNext(SBServer server) { return NULL; }
int SBServerGetPing(SBServer server) { return (int)0; }
double SBServerGetPlayerFloatValueA(SBServer server, int playernum, const char *key, double fdefault) { return (double)0; }
int SBServerGetPlayerIntValueA(SBServer server, int playernum, const char *key, int idefault) { return (int)0; }
const char * SBServerGetPlayerStringValueA(SBServer server, int playernum, const char *key, const char *sdefault) { return NULL; }
char * SBServerGetPrivateAddress(SBServer server) { return NULL; }
unsigned int SBServerGetPrivateInetAddress(SBServer server) { return (unsigned int)0; }
unsigned short SBServerGetPrivateQueryPort(SBServer server) { return (unsigned short)0; }
char * SBServerGetPublicAddress(SBServer server) { return NULL; }
unsigned int SBServerGetPublicInetAddress(SBServer server) { return (unsigned int)0; }
unsigned short SBServerGetPublicQueryPort(SBServer server) { return (unsigned short)0; }
unsigned short SBServerGetPublicQueryPortNBO(SBServer server) { return (unsigned short)0; }
unsigned char SBServerGetState(SBServer server) { return (unsigned char)0; }
const char * SBServerGetStringValueA(SBServer server, const char *keyname, const char *def) { return def; }
double SBServerGetTeamFloatValueA(SBServer server, int teamnum, const char *key, double fdefault) { return (double)0; }
int SBServerGetTeamIntValueA(SBServer server, int teamnum, const char *key, int idefault) { return (int)0; }
const char * SBServerGetTeamStringValueA(SBServer server, int teamnum, const char *key, const char *sdefault) { return NULL; }
SBBool SBServerHasBasicKeys(SBServer server) { return (SBBool)0; }
SBBool SBServerHasFullKeys(SBServer server) { return (SBBool)0; }
SBBool SBServerHasPrivateAddress(SBServer server) { return (SBBool)0; }
SBBool SBServerHasValidPing(SBServer server) { return (SBBool)0; }
void SBServerListAppendServer(SBServerList *slist, SBServer server) {  }
void SBServerListCleanup(SBServerList *slist) {  }
void SBServerListClear(SBServerList *slist) {  }
SBError SBServerListConnectAndQuery(SBServerList *slist, const char *fieldList, const char *serverFilter, int options, int maxServers) { return (SBError)0; }
int SBServerListCount(SBServerList *slist) { return (int)0; }
void SBServerListDisconnect(SBServerList *slist) {  }
int SBServerListFindServer(SBServerList *slist, SBServer findserver) { return (int)0; }
int SBServerListFindServerByIP(SBServerList *slist, goa_uint32 ip, unsigned short port) { return (int)0; }
SBError SBServerListGetLANList(SBServerList *slist, unsigned short startport, unsigned short endport, int queryversion) { return (SBError)0; }
void SBServerListInit(SBServerList *slist, const char *queryForGamename, const char *queryFromGamename, const char *queryFromKey, int queryFromVersion, SBBool lanBrowse, SBListCallBackFn callback, void *instance) {  }
SBServer SBServerListNth(SBServerList *slist, int i) { return NULL; }
void SBServerListRemoveAt(SBServerList *slist, int index) {  }
void SBServerListSort(SBServerList *slist, SBBool ascending, SortInfo sortinfo) {  }
void SBServerParseKeyVals(SBServer server, char *keyvals) {  }
void SBServerParseQR2FullKeysSingle(SBServer server, char *data, int len) {  }
void SBServerParseQR2FullKeysSplit(SBServer server, char *data, int len) {  }
void SBServerSetFlags(SBServer server, unsigned char flags) {  }
void SBServerSetICMPIP(SBServer server, goa_uint32 icmpip) {  }
void SBServerSetNext(SBServer server, SBServer next) {  }
void SBServerSetPrivateAddr(SBServer server, goa_uint32 ip, unsigned short port) {  }
void SBServerSetPublicAddr(SBServer server, goa_uint32 ip, unsigned short port) {  }
void SBServerSetState(SBServer server, unsigned char state) {  }
int SendGameSnapShotA(statsgame_t game, const char *snapshot, int final) { return (int)0; }
void SetPersistData(int localid, int profileid, persisttype_t type, int index, const char *data, int len, PersDataSaveCallbackFn callback, void *instance) {  }
void SetPersistDataValuesA(int localid, int profileid, persisttype_t type, int index, const char *keyvalues, PersDataSaveCallbackFn callback, void *instance) {  }
int StatsThink() { return GE_NOCONNECT; }
void ghttpCancelRequest(GHTTPRequest request) {  }
void ghttpCleanup(void) {  }
void ghttpCloseRequest(GHTTPRequest request) {  }
void ghttpFreePost(GHTTPPost post) {  }
GHTTPRequest ghttpGetA(const char * URL, GHTTPBool blocking, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
GHTTPRequest ghttpGetExA(const char * URL, const char * headers, char * buffer, int bufferSize, GHTTPPost post, GHTTPBool throttle, GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
const char * ghttpGetHeaders(GHTTPRequest request) { return NULL; }
const char * ghttpGetResponseStatus(GHTTPRequest request, int * statusCode) { return NULL; }
GHTTPState ghttpGetState(GHTTPRequest request) { return (GHTTPState)-1; }
const char * ghttpGetURL(GHTTPRequest request) { return NULL; }
GHTTPRequest ghttpHeadA(const char * URL, GHTTPBool blocking, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
GHTTPRequest ghttpHeadExA(const char * URL, const char * headers, GHTTPBool throttle, GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
GHTTPPost ghttpNewPost(void) { return (GHTTPPost)-1; }
GHTTPRequest ghttpPostA(const char * URL, GHTTPPost post, GHTTPBool blocking, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
GHTTPBool ghttpPostAddFileFromDiskA(GHTTPPost post, const char * name, const char * filename, const char * reportFilename, const char * contentType) { return (GHTTPBool)0; }
GHTTPBool ghttpPostAddFileFromMemoryA(GHTTPPost post, const char * name, const char * buffer, int bufferLen, const char * reportFilename, const char * contentType) { return (GHTTPBool)0; }
GHTTPBool ghttpPostAddStringA(GHTTPPost post, const char * name, const char * string) { return (GHTTPBool)0; }
GHTTPBool ghttpPostAddXml(GHTTPPost post, GSXmlStreamWriter xmlSoap) { return (GHTTPBool)0; }
GHTTPRequest ghttpPostExA(const char * URL, const char * headers, GHTTPPost post, GHTTPBool throttle, GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
void ghttpPostSetAutoFree(GHTTPPost post, GHTTPBool autoFree) {  }
void ghttpPostSetCallback(GHTTPPost post, ghttpPostCallback callback, void * param) {  }
GHTTPBool ghttpRequestThink(GHTTPRequest request) { return (GHTTPBool)0; }
GHTTPRequest ghttpSaveA(const char * URL, const char * filename, GHTTPBool blocking, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
GHTTPRequest ghttpSaveExA(const char * URL, const char * filename, const char * headers, GHTTPPost post, GHTTPBool throttle, GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
void ghttpSetMaxRecvTime(GHTTPRequest request, gsi_time maxRecvTime) {  }
GHTTPBool ghttpSetProxy(const char * server) { return (GHTTPBool)0; }
GHTTPBool ghttpSetRequestEncryptionEngine(GHTTPRequest request, GHTTPEncryptionEngine engine) { return (GHTTPBool)0; }
GHTTPBool ghttpSetRequestProxy(GHTTPRequest request, const char * server) { return (GHTTPBool)0; }
void ghttpSetThrottle(GHTTPRequest request, GHTTPBool throttle) {  }
void ghttpStartup(void) {  }
GHTTPRequest ghttpStreamA(const char * URL, GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
GHTTPRequest ghttpStreamExA(const char * URL, const char * headers, GHTTPPost post, GHTTPBool throttle, GHTTPBool blocking, ghttpProgressCallback progressCallback, ghttpCompletedCallback completedCallback, void * param) { return (GHTTPRequest)-1; }
void ghttpThink(void) {  }
void ghttpThrottleSettings(int bufferSize, gsi_time timeDelay) {  }
GPResult gpAcceptTransferA(GPConnection * connection, GPTransfer transfer, const char * message) { return GP_NETWORK_ERROR; }
GPResult gpAddToBlockedList(GPConnection * connection, GPProfile profile) { return GP_NETWORK_ERROR; }
GPResult gpAuthBuddyRequest(GPConnection * connection, GPProfile profile) { return GP_NETWORK_ERROR; }
GPResult gpCheckUserA(GPConnection * connection, const char nick[31], const char email[51], const char password[31], GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpConnectA(GPConnection * connection, const char nick[31], const char email[51], const char password[31], GPEnum firewall, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpConnectNewUserA(GPConnection * connection, const char nick[31], const char uniquenick[21], const char email[51], const char password[31], const char cdkey[65], GPEnum firewall, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpConnectPreAuthenticatedA(GPConnection * connection, const char authtoken[256], const char partnerchallenge[256], GPEnum firewall, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpConnectUniqueNickA(GPConnection * connection, const char uniquenick[21], const char password[31], GPEnum firewall, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpDeleteBuddy(GPConnection * connection, GPProfile profile) { return GP_NETWORK_ERROR; }
GPResult gpDeleteProfile(GPConnection * connection, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpDenyBuddyRequest(GPConnection * connection, GPProfile profile) { return GP_NETWORK_ERROR; }
void gpDestroy(GPConnection * connection) {  }
GPResult gpDisable(GPConnection * connection, GPEnum state) { return GP_NETWORK_ERROR; }
void gpDisconnect(GPConnection * connection) {  }
GPResult gpEnable(GPConnection * connection, GPEnum state) { return GP_NETWORK_ERROR; }
GPResult gpFindPlayers(GPConnection * connection, int productID, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpFreeTransfer(GPConnection * connection, GPTransfer transfer) { return GP_NETWORK_ERROR; }
GPResult gpGetBlockedProfile(GPConnection * connection, int index, GPProfile * profile) { return GP_NETWORK_ERROR; }
GPResult gpGetBuddyIndex(GPConnection * connection, GPProfile profile, int * index) { return GP_NETWORK_ERROR; }
GPResult gpGetBuddyStatus(GPConnection * connection, int index, GPBuddyStatus * status) { return GP_NETWORK_ERROR; }
GPResult gpGetCurrentFile(GPConnection * connection, GPTransfer transfer, int * index) { return GP_NETWORK_ERROR; }
GPResult gpGetErrorCode(GPConnection * connection, GPErrorCode * errorCode) { return GP_NETWORK_ERROR; }
GPResult gpGetErrorStringA(GPConnection * connection, char errorString[256]) { return GP_NETWORK_ERROR; }
GPResult gpGetFileModificationTime(GPConnection * connection, GPTransfer transfer, int index, gsi_time * modTime) { return GP_NETWORK_ERROR; }
GPResult gpGetFileNameA(GPConnection * connection, GPTransfer transfer, int index, char ** name) { return GP_NETWORK_ERROR; }
GPResult gpGetFilePathA(GPConnection * connection, GPTransfer transfer, int index, char ** path) { return GP_NETWORK_ERROR; }
GPResult gpGetFileProgress(GPConnection * connection, GPTransfer transfer, int index, int * progress) { return GP_NETWORK_ERROR; }
GPResult gpGetFileSize(GPConnection * connection, GPTransfer transfer, int index, int * size) { return GP_NETWORK_ERROR; }
GPResult gpGetInfo(GPConnection * connection, GPProfile profile, GPEnum checkCache, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpGetInfoNoWait(GPConnection * connection, GPProfile profile, GPGetInfoResponseArg * arg) { return GP_NETWORK_ERROR; }
GPResult gpGetLoginTicket(GPConnection * connection, char loginTicket[25]) { return GP_NETWORK_ERROR; }
GPResult gpGetNumBlocked(GPConnection * connection, int * numBlocked) { return GP_NETWORK_ERROR; }
GPResult gpGetNumBuddies(GPConnection * connection, int * numBuddies) { return GP_NETWORK_ERROR; }
GPResult gpGetNumFiles(GPConnection * connection, GPTransfer transfer, int * num) { return GP_NETWORK_ERROR; }
GPResult gpGetNumTransfers(GPConnection * connection, int * num) { return GP_NETWORK_ERROR; }
GPResult gpGetReversBuddiesList(GPConnection * connection, GPProfile *targets, int numOfTargets, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpGetReverseBuddies(GPConnection * connection, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpGetTransfer(GPConnection * connection, int index, GPTransfer * transfer) { return GP_NETWORK_ERROR; }
void * gpGetTransferData(GPConnection * connection, GPTransfer transfer) { return NULL; }
GPResult gpGetTransferProfile(GPConnection * connection, GPTransfer transfer, GPProfile * profile) { return GP_NETWORK_ERROR; }
GPResult gpGetTransferProgress(GPConnection * connection, GPTransfer transfer, int * progress) { return GP_NETWORK_ERROR; }
GPResult gpGetTransferSide(GPConnection * connection, GPTransfer transfer, GPEnum * side) { return GP_NETWORK_ERROR; }
GPResult gpGetTransferSize(GPConnection * connection, GPTransfer transfer, int * size) { return GP_NETWORK_ERROR; }
GPResult gpGetTransferThrottle(GPConnection * connection, GPTransfer transfer, int * throttle) { return GP_NETWORK_ERROR; }
GPResult gpGetUserNicksA(GPConnection * connection, const char email[51], const char password[31], GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpIDFromProfile(GPConnection * connection, GPProfile profile, int * id) { return GP_NETWORK_ERROR; }
GPResult gpInitialize(GPConnection * connection, int productID, int namespaceID, int partnerID) { *connection = NULL; return GP_NETWORK_ERROR; }
GPResult gpInvitePlayerA(GPConnection * connection, GPProfile profile, int productID, const char location[256]) { return GP_NETWORK_ERROR; }
gsi_bool gpIsBlocked(GPConnection * connection, GPProfile profile) { return (gsi_bool)0; }
int gpIsBuddy(GPConnection * connection, GPProfile profile) { return (int)0; }
int gpIsBuddyConnectionOpen(GPConnection * connection, GPProfile profile) { return (int)0; }
GPResult gpIsConnected(GPConnection * connection, GPEnum * connected) { return GP_NETWORK_ERROR; }
GPResult gpIsValidEmailA(GPConnection * connection, const char email[51], GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpNewProfileA(GPConnection * connection, const char nick[31], GPEnum replace, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpNewUserA(GPConnection * connection, const char nick[31], const char uniquenick[21], const char email[51], const char password[31], const char cdkey[65], GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpProcess(GPConnection * connection) { return GP_NETWORK_ERROR; }
GPResult gpProfileFromID(GPConnection * connection, GPProfile * profile, int id) { return GP_NETWORK_ERROR; }
GPResult gpProfileSearchA(GPConnection * connection, const char nick[31], const char uniquenick[21], const char email[51], const char firstname[31], const char lastname[31], int icquin, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpProfileSearchUniquenickA(GPConnection * connection, const char uniquenick[21], const int namespaceIDs[16], int numNamespaces, GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpRegisterCdKeyA(GPConnection * connection, const char cdkey[65], GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpRegisterUniqueNickA(GPConnection * connection, const char uniquenick[21], const char cdkey[65], GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpRejectTransferA(GPConnection * connection, GPTransfer transfer, const char * message) { return GP_NETWORK_ERROR; }
GPResult gpRemoveFromBlockedList(GPConnection * connection, GPProfile profile) { return GP_NETWORK_ERROR; }
GPResult gpRevokeBuddyAuthorization(GPConnection * connection, GPProfile profile) { return GP_NETWORK_ERROR; }
GPResult gpSendBuddyMessageA(GPConnection * connection, GPProfile profile, const char * message) { return GP_NETWORK_ERROR; }
GPResult gpSendBuddyRequestA(GPConnection * connection, GPProfile profile, const char reason[1025]) { return GP_NETWORK_ERROR; }
GPResult gpSendBuddyUTMA(GPConnection * connection, GPProfile profile, const char * message, int sendOption) { return GP_NETWORK_ERROR; }
GPResult gpSendFilesA(GPConnection * connection, GPTransfer * transfer, GPProfile profile, const char * message, gpSendFilesCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpSetCallback(GPConnection * connection, GPEnum func, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpSetCdKey(GPConnection * connection, const char cdkeyhash, GPCallback callback) { return GP_NETWORK_ERROR; }
void gpSetInfoCacheFilenameA(const char * filename) {  }
GPResult gpSetInfoMask(GPConnection * connection, GPEnum mask) { return GP_NETWORK_ERROR; }
GPResult gpSetInfod(GPConnection * connection, GPEnum info, int day, int month, int year) { return GP_NETWORK_ERROR; }
GPResult gpSetInfoi(GPConnection * connection, GPEnum info, int value) { return GP_NETWORK_ERROR; }
GPResult gpSetInfosA(GPConnection * connection, GPEnum info, const char * value) { return GP_NETWORK_ERROR; }
GPResult gpSetInvitableGames(GPConnection * connection, int numProductIDs, const int * productIDs) { return GP_NETWORK_ERROR; }
GPResult gpSetQuietMode(GPConnection * connection, GPEnum flags) { return GP_NETWORK_ERROR; }
GPResult gpSetStatusA(GPConnection * connection, GPEnum status, const char statusString[256], const char locationString[256]) { return GP_NETWORK_ERROR; }
GPResult gpSetTransferData(GPConnection * connection, GPTransfer transfer, void * userData) { return GP_NETWORK_ERROR; }
GPResult gpSetTransferDirectoryA(GPConnection * connection, GPTransfer transfer, const char * directory) { return GP_NETWORK_ERROR; }
GPResult gpSetTransferThrottle(GPConnection * connection, GPTransfer transfer, int throttle) { return GP_NETWORK_ERROR; }
GPResult gpSkipFile(GPConnection * connection, GPTransfer transfer, int index) { return GP_NETWORK_ERROR; }
GPResult gpSuggestUniqueNickA(GPConnection * connection, const char desirednick[21], GPEnum blocking, GPCallback callback, void * param) { return GP_NETWORK_ERROR; }
GPResult gpUserIDFromProfile(GPConnection * connection, GPProfile profile, int * userid) { return GP_NETWORK_ERROR; }
void peerAlwaysGetPlayerInfo(PEER peer, PEERBool always) {  }
PEERBool peerAreAllReady(PEER peer) { return (PEERBool)0; }
void peerAuthenticateCDKeyA(PEER peer, const char * cdkey, peerAuthenticateCDKeyCallback callback, void * param, PEERBool blocking) {  }
void peerChangeNickA(PEER peer, const char * newNick, peerChangeNickCallback callback, void * param, PEERBool blocking) {  }
void peerClearTitle(PEER peer) {  }
void peerConnectA(PEER peer, const char * nick, int profileID, peerNickErrorCallback nickErrorCallback, peerConnectCallback connectCallback, void * param, PEERBool blocking) {  }
void peerConnectLoginA(PEER peer, int namespaceID, const char * email, const char * profilenick, const char * uniquenick, const char * password, peerNickErrorCallback nickErrorCallback, peerConnectCallback connectCallback, void * param, PEERBool blocking) {  }
void peerConnectPreAuthA(PEER peer, const char * authtoken, const char * partnerchallenge, peerNickErrorCallback nickErrorCallback, peerConnectCallback connectCallback, void * param, PEERBool blocking) {  }
void peerCreateStagingRoomA(PEER peer, const char * name, int maxPlayers, const char password[24], peerJoinRoomCallback callback, void * param, PEERBool blocking) {  }
void peerCreateStagingRoomWithSocketA(PEER peer, const char * name, int maxPlayers, const char password[24], SOCKET socket, unsigned short port, peerJoinRoomCallback callback, void * param, PEERBool blocking) {  }
void peerDisconnect(PEER peer) {  }
void peerEnumPlayers(PEER peer, RoomType roomType, peerEnumPlayersCallback callback, void * param) {  }
void peerFixNickA(char * newNick, const char * oldNick) {  }
PEERAutoMatchStatus peerGetAutoMatchStatus(PEER peer) { return (PEERAutoMatchStatus)0; }
CHAT peerGetChat(PEER peer) { return (CHAT)0; }
const char * peerGetGlobalWatchKeyA(PEER peer, const char * nick, const char * key) { return NULL; }
int peerGetGroupID(PEER peer) { return (int)0; }
SBServer peerGetHostServer(PEER peer) { return NULL; }
const char * peerGetNickA(PEER peer) { return NULL; }
PEERBool peerGetPlayerFlagsA(PEER peer, const char * nick, RoomType roomType, int * flags) { return (PEERBool)0; }
void peerGetPlayerGlobalKeysA(PEER peer, const char * nick, int num, const char ** keys, peerGetGlobalKeysCallback callback, void * param, PEERBool blocking) {  }
void peerGetPlayerIPA(PEER peer, const char * nick, peerGetPlayerIPCallback callback, void * param, PEERBool blocking) {  }
void peerGetPlayerInfoA(PEER peer, const char * nick, peerGetPlayerInfoCallback callback, void * param, PEERBool blocking) {  }
PEERBool peerGetPlayerInfoNoWaitA(PEER peer, const char * nick, unsigned int * IP, int * profileID) { return (PEERBool)0; }
PEERBool peerGetPlayerPingA(PEER peer, const char * nick, int * ping) { return (PEERBool)0; }
void peerGetPlayerProfileIDA(PEER peer, const char * nick, peerGetPlayerProfileIDCallback callback, void * param, PEERBool blocking) {  }
PEERBool peerGetPlayersCrossPingA(PEER peer, const char * nick1, const char * nick2, int * crossPing) { return (PEERBool)0; }
unsigned int peerGetPrivateIP(PEER peer) { return (unsigned int)0; }
int peerGetProfileID(PEER peer) { return (int)0; }
unsigned int peerGetPublicIP(PEER peer) { return (unsigned int)0; }
PEERBool peerGetReadyA(PEER peer, const char * nick, PEERBool * ready) { return (PEERBool)0; }
const char * peerGetRoomChannelA(PEER peer, RoomType roomType) { return NULL; }
void peerGetRoomGlobalKeysA(PEER peer, RoomType roomType, int num, const char ** keys, peerGetGlobalKeysCallback callback, void * param, PEERBool blocking) {  }
void peerGetRoomKeysA(PEER peer, RoomType roomType, const char * nick, int num, const char ** keys, peerGetRoomKeysCallback callback, void * param, PEERBool blocking) {  }
const char * peerGetRoomNameA(PEER peer, RoomType roomType) { return NULL; }
const char * peerGetRoomWatchKeyA(PEER peer, RoomType roomType, const char * nick, const char * key) { return NULL; }
const char * peerGetTitleA(PEER peer) { return NULL; }
int peerGetUserID(PEER peer) { return (int)0; }
PEERBool peerInRoom(PEER peer, RoomType roomType) { return (PEERBool)0; }
PEER peerInitialize(PEERCallbacks * callbacks) { return NULL; }
PEERBool peerIsAutoMatching(PEER peer) { return (PEERBool)0; }
PEERBool peerIsConnected(PEER peer) { return (PEERBool)0; }
PEERBool peerIsPlayerHostA(PEER peer, const char * nick, RoomType roomType) { return (PEERBool)0; }
PEERBool peerIsPlaying(PEER peer) { return (PEERBool)0; }
void peerJoinGroupRoom(PEER peer, int groupID, peerJoinRoomCallback callback, void * param, PEERBool blocking) {  }
void peerJoinStagingRoomA(PEER peer, SBServer server, const char password[24], peerJoinRoomCallback callback, void * param, PEERBool blocking) {  }
void peerJoinStagingRoomByChannelA(PEER peer, const char * channel, const char password[24], peerJoinRoomCallback callback, void * param, PEERBool blocking) {  }
void peerJoinTitleRoomA(PEER peer, const char password[24], peerJoinRoomCallback callback, void * param, PEERBool blocking) {  }
void peerKickPlayerA(PEER peer, RoomType roomType, const char * nick, const char * reason) {  }
void peerLeaveRoomA(PEER peer, RoomType roomType, const char * reason) {  }
void peerListGroupRoomsA(PEER peer, const char * fields, peerListGroupRoomsCallback callback, void * param, PEERBool blocking) {  }
void peerMessagePlayerA(PEER peer, const char * nick, const char * message, MessageType messageType) {  }
void peerMessageRoomA(PEER peer, RoomType roomType, const char * message, MessageType messageType) {  }
void peerParseQueryA(PEER peer, char * query, int len, struct sockaddr * sender) {  }
PEERBool peerPingPlayerA(PEER peer, const char * nick) { return (PEERBool)0; }
void peerRegisterUniqueNickA(PEER peer, int namespaceID, const char * uniquenick, const char * cdkey) {  }
void peerRetryWithNickA(PEER peer, const char * nick) {  }
void peerSendMessageToServer(PEER peer, unsigned int ip, unsigned short port, const char * data, int len) {  }
void peerSendNatNegotiateCookie(PEER peer, unsigned int ip, unsigned short port, int cookie) {  }
void peerSetAwayModeA(PEER peer, const char * reason) {  }
void peerSetGlobalKeysA(PEER peer, int num, const char ** keys, const char ** values) {  }
void peerSetGlobalWatchKeysA(PEER peer, RoomType roomType, int num, const char ** keys, PEERBool addKeys) {  }
void peerSetGroupID(PEER peer, int groupID) {  }
void peerSetPasswordA(PEER peer, RoomType roomType, const char password[24]) {  }
void peerSetQuietMode(PEER peer, PEERBool quiet) {  }
void peerSetReady(PEER peer, PEERBool ready) {  }
void peerSetRoomKeysA(PEER peer, RoomType roomType, const char * nick, int num, const char ** keys, const char ** values) {  }
void peerSetRoomNameA(PEER peer, RoomType roomType, const char * name) {  }
void peerSetRoomWatchKeysA(PEER peer, RoomType roomType, int num, const char ** keys, PEERBool addKeys) {  }
void peerSetStagingRoomMaxPlayers(PEER peer, int maxPlayers) {  }
PEERBool peerSetTitleA(PEER peer, const char * title, const char * qrSecretKey, const char * sbName, const char * sbSecretKey, int sbGameVersion, int sbMaxUpdates, PEERBool natNegotiate, PEERBool pingRooms[NumRooms], PEERBool crossPingRooms[NumRooms]) { return (PEERBool)0; }
void peerSetTitleRoomChannelA(PEER peer, const char * channel) {  }
void peerShutdown(PEER peer) {  }
void peerStartAutoMatchA(PEER peer, int maxPlayers, const char * filter, peerAutoMatchStatusCallback statusCallback, peerAutoMatchRateCallback rateCallback, void * param, PEERBool blocking) {  }
void peerStartAutoMatchWithSocketA(PEER peer, int maxPlayers, const char * filter, SOCKET socket, unsigned short port, peerAutoMatchStatusCallback statusCallback, peerAutoMatchRateCallback rateCallback, void * param, PEERBool blocking) {  }
void peerStartGameA(PEER peer, const char * message, int reportingOptions) {  }
void peerStartListingGamesA(PEER peer, const unsigned char * fields, int numFields, const char * filter, peerListingGamesCallback callback, void * param) {  }
void peerStartPlaying(PEER peer) {  }
PEERBool peerStartReporting(PEER peer) { return (PEERBool)0; }
PEERBool peerStartReportingWithSocket(PEER peer, SOCKET socket, unsigned short port) { return (PEERBool)0; }
void peerStateChanged(PEER peer) {  }
void peerStayInRoom(PEER peer, RoomType roomType) {  }
void peerStopAutoMatch(PEER peer) {  }
void peerStopGame(PEER peer) {  }
void peerStopListingGames(PEER peer) {  }
void peerThink(PEER peer) {  }
const char * peerTranslateNickA(char * nick, const char * extension) { return NULL; }
void peerUTMPlayerA(PEER peer, const char * nick, const char * command, const char * parameters, PEERBool authenticate) {  }
void peerUTMRoomA(PEER peer, RoomType roomType, const char * command, const char * parameters, PEERBool authenticate) {  }
void peerUpdateGame(PEER peer, SBServer server, PEERBool fullUpdate) {  }
void peerUpdateGameByMaster(PEER peer, SBServer server, PEERBool fullUpdate) {  }
void peerUpdateGamePing(PEER peer, SBServer server) {  }
PTBool ptCheckForPatchA(int productID, const char * versionUniqueID, int distributionID, ptPatchCallback callback, PTBool blocking, void * param) { return (PTBool)0; }
PTBool ptCheckForPatchAndTrackUsageA(int userID, int productID, const char * versionUniqueID, int distributionID, ptPatchCallback callback, PTBool blocking, void * param) { return (PTBool)0; }
PTBool ptCreateCheckPatchTrackUsageReqA(int userID, int productID, const char * versionUniqueID, int distributionID, ptPatchCallback callback, PTBool blocking, int *request, void * param) { return (PTBool)0; }
PTBool ptLookupFilePlanetInfo(int fileID, ptFilePlanetInfoCallback callback, PTBool blocking, void * param) { return (PTBool)0; }
PTBool ptTrackUsageA(int userID, int productID, const char * versionUniqueID, int distributionID, PTBool blocking) { return (PTBool)0; }
gsi_bool qr2_buffer_addA(qr2_buffer_t outbuf, const char *value) { return (gsi_bool)0; }
gsi_bool qr2_buffer_add_int(qr2_buffer_t outbuf, int value) { return (gsi_bool)0; }
qr2_error_t qr2_initA(qr2_t *qrec, const char *ip, int baseport, const char *gamename, const char *secret_key, int ispublic, int natnegotiate, qr2_serverkeycallback_t server_key_callback, qr2_playerteamkeycallback_t player_key_callback, qr2_playerteamkeycallback_t team_key_callback, qr2_keylistcallback_t key_list_callback, qr2_countcallback_t playerteam_count_callback, qr2_adderrorcallback_t adderror_callback, void *userdata) { return (qr2_error_t)0; }
qr2_error_t qr2_init_socketA(qr2_t *qrec, SOCKET s, int boundport, const char *gamename, const char *secret_key, int ispublic, int natnegotiate, qr2_serverkeycallback_t server_key_callback, qr2_playerteamkeycallback_t player_key_callback, qr2_playerteamkeycallback_t team_key_callback, qr2_keylistcallback_t key_list_callback, qr2_countcallback_t playerteam_count_callback, qr2_adderrorcallback_t adderror_callback, void *userdata) { return (qr2_error_t)0; }
gsi_bool qr2_internal_is_master_only_key(const char * keyname) { return (gsi_bool)0; }
void qr2_internal_key_list_append(char* theKey) {  }
void qr2_internal_key_list_free() {  }
gsi_bool qr2_keybuffer_add(qr2_keybuffer_t keybuffer, int keyid) { return (gsi_bool)0; }
void qr2_parse_queryA(qr2_t qrec, char *query, int len, struct sockaddr *sender) {  }
void qr2_register_clientconnected_callback(qr2_t qrec, qr2_clientconnectedcallback_t cccallback) {  }
void qr2_register_clientmessage_callback(qr2_t qrec, qr2_clientmessagecallback_t cmcallback) {  }
void qr2_register_denyresponsetoip_callback(qr2_t qrec, qr2_denyqr2responsetoipcallback_t dertoipcallback) {  }
void qr2_register_keyA(int keyid, const char *key) {  }
void qr2_register_keyW(int keyid, const unsigned short *key) {  }
void qr2_register_natneg_callback(qr2_t qrec, qr2_natnegcallback_t nncallback) {  }
void qr2_register_publicaddress_callback(qr2_t qrec, qr2_publicaddresscallback_t pacallback) {  }
void qr2_send_statechanged(qr2_t qrec) {  }
void qr2_shutdown(qr2_t qrec) {  }
void qr2_think(qr2_t qrec) {  }
}
