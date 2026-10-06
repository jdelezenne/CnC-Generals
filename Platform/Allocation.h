// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>
#include <new>

// The C++ runtime supplies the standard and placement operators. Keep the
// game's file/line overloads available without redeclaring those operators.
#ifndef _OPERATOR_NEW_DEFINED_
#define _OPERATOR_NEW_DEFINED_
#endif
void* operator new(std::size_t size, const char* file, int line);
void operator delete(void* pointer, const char* file, int line);
void* operator new[](std::size_t size, const char* file, int line);
void operator delete[](void* pointer, const char* file, int line);
