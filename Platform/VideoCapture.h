// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>
namespace Platform {
struct VideoCapture;
VideoCapture* OpenVideoCapture(const char* filename, int width, int height, int bits, int rate);
std::size_t VideoCaptureFrameSize(const VideoCapture* capture);
bool WriteVideoCapture(VideoCapture* capture, const void* pixels);
void CloseVideoCapture(VideoCapture* capture);
}
