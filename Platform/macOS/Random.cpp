// SPDX-License-Identifier: GPL-3.0-or-later
#include "srandom.h"
#include <cstdlib>
void SecureRandomClass::Generate_Seed()
{
    arc4random_buf(Seeds, sizeof(Seeds));
}
