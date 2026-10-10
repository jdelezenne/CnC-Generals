/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "framgrab.h"
#include "Platform/Directory.h"
#include "Platform/Memory.h"
#include "Platform/System.h"
#include <cstdio>
#include <cstdint>

FrameGrabClass::FrameGrabClass(const char* filename, MODE mode, int width, int height, int bitcount, float framerate)
    : Filename(filename), FrameRate(framerate), Mode(mode), Counter(0), Capture(NULL), Bitmap(NULL), Width(width), Height(height)
{
    if (Mode != AVI) return;
    int counter = 0;
    char file[256];
    do {
        sprintf(file, "%s%d.AVI", filename, counter++);
    } while (Platform::PathExists(file));
    Capture = Platform::OpenVideoCapture(file, width, height, bitcount, static_cast<int>(framerate));
    if (Capture) Bitmap = static_cast<long*>(Platform::AllocateSystemMemory(Platform::VideoCaptureFrameSize(Capture), false));
    if (!Capture || !Bitmap) {
        Platform::DebugMonitorOutput("Unable to create AVI capture\n");
        CleanupAVI();
    }
}

FrameGrabClass::~FrameGrabClass()
{
    if (Mode == AVI) CleanupAVI();
}

void FrameGrabClass::CleanupAVI()
{
    Platform::FreeSystemMemory(Bitmap);
    Bitmap = NULL;
    Platform::CloseVideoCapture(Capture);
    Capture = NULL;
    Mode = RAW;
}

void FrameGrabClass::GrabAVI(void* pixels)
{
    ++Counter;
    if (!Platform::WriteVideoCapture(Capture, pixels)) Platform::DebugMonitorOutput("AVI write error\n");
}

void FrameGrabClass::GrabRawFrame(void*) {}

void FrameGrabClass::ConvertGrab(void* pixels)
{
    ConvertFrame(pixels);
    Grab(Bitmap);
}

void FrameGrabClass::Grab(void* pixels)
{
    if (Mode == AVI) GrabAVI(pixels);
    else GrabRawFrame(pixels);
}

void FrameGrabClass::ConvertFrame(void* pixels)
{
    // This legacy conversion operates on 32-bit pixels, including on LP64 hosts.
    auto* image = static_cast<std::uint32_t*>(pixels);
    auto* bitmap = reinterpret_cast<std::uint32_t*>(Bitmap);
    int y = Height;
    while (y--) {
        int x = Width;
        const int yoffset = y * Width;
        const int yoffset2 = (Height - y) * Width;
        while (x--) {
            auto* destination = &bitmap[yoffset2 + x];
            *destination = image[yoffset + x];
            auto* c = reinterpret_cast<unsigned char*>(destination);
            c[3] = c[0]; c[0] = c[2]; c[2] = c[3]; c[3] = 0;
        }
    }
}
