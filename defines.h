#pragma once

#include <iostream>
#include <b15f/b15f.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <bitset>
#include <unordered_map>

#ifdef SEND
    #define BITSETTING 0x0F
#else
    #define BITSETTING 0x00
#endif


#define BIT_PERIOD 500
#define CONTROL_BIT1 0b0001