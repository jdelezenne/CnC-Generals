// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#ifndef _MSC_VER
// Preserve the MSVC intrinsic selection, including NaN and argument evaluation.
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#define __max(a,b) (((a) > (b)) ? (a) : (b))
#endif
