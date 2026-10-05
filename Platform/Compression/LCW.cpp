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

#include "LCW.h"
#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <vector>

int Platform::CompressLCW(void const * source, void * dest, int datasize)
{
    if (datasize <= 0) { *static_cast<unsigned char*>(dest) = 0x80; return 1; }
    const auto* input = static_cast<const unsigned char*>(source);
    auto* output = static_cast<unsigned char*>(dest);
    auto* begin = output;
    std::unordered_map<unsigned, int> last;
    std::vector<int> previous(datasize, -1);
    auto key = [&](int i) { return input[i] | (unsigned(input[i+1])<<8) | (unsigned(input[i+2])<<16); };
    auto remember = [&](int first, int end) {
        for (int i = first; i < end && i+2 < datasize; ++i) {
            unsigned value = key(i);
            auto found = last.find(value);
            previous[i] = found == last.end() ? -1 : found->second;
            last[value] = i;
        }
    };
    auto word = [&](unsigned value) { *output++ = static_cast<unsigned char>(value); *output++ = static_cast<unsigned char>(value>>8); };
    int literal = 0, position = 0;
    auto flush = [&] {
        while (literal < position) {
            int count = std::min(63, position-literal);
            *output++ = static_cast<unsigned char>(0x80|count);
            std::memcpy(output, input+literal, count); output += count; literal += count;
        }
    };
    while (position < datasize) {
        int run = 1;
        while (run < 65535 && position+run < datasize && input[position+run] == input[position]) ++run;
        if (run >= 65) {
            flush(); *output++ = 0xfe; word(run); *output++ = input[position];
            remember(position, position+run); position += run; literal = position; continue;
        }
        int length = 0, match = 0;
        if (position+2 < datasize) {
            auto found = last.find(key(position));
            if (found != last.end()) for (int candidate = found->second; candidate >= 0; candidate = previous[candidate]) {
                if (candidate > 65535) continue;
                int count = 3;
                while (count < 65535 && position+count < datasize && input[candidate+count] == input[position+count]) ++count;
                if (count > length) { length = count; match = candidate; }
                if (position+count == datasize) break;
            }
        }
        unsigned offset = position-match;
        if (length >= 4 || (length == 3 && offset < 4096)) {
            flush();
            if (length <= 10 && offset < 4096) { *output++ = static_cast<unsigned char>(((length-3)<<4)|(offset>>8)); *output++ = static_cast<unsigned char>(offset); }
            else if (length <= 64) { *output++ = static_cast<unsigned char>(0xc0|(length-3)); word(match); }
            else { *output++ = 0xff; word(length); word(match); }
            remember(position, position+length); position += length; literal = position;
        } else { remember(position, position+1); ++position; }
    }
    flush(); *output++ = 0x80;
    return static_cast<int>(output-begin);
}
