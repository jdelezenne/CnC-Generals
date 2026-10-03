// The game still loads video metadata, but unavailable movies return no stream.
#include "PreRTS.h"
#include "VideoDevice/Bink/BinkVideoPlayer.h"

VideoStreamInterface *BinkVideoPlayer::open(AsciiString) { return NULL; }
VideoStreamInterface *BinkVideoPlayer::load(AsciiString) { return NULL; }
void BinkVideoPlayer::notifyVideoPlayerOfNewProvider(Bool) {}
void BinkVideoPlayer::initializeBinkWithMiles() {}
