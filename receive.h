#pragma once
#include "defines.h"

void waitForStartBit(B15F& drv);
void getData(B15F& drv);
void writeToBuffer(std::vector<std::bitset<4>> *buffer, B15F& drv);
std::vector<std::bitset<4>>* cleanSonderzeichen(std::vector<std::bitset<4>> *buffer);
void processBuffer(std::vector<std::bitset<4>> *buffer);
