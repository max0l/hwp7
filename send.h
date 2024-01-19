#pragma once
#include "defines.h"

std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input);
void sendData(B15F& drv);
void sendStartSequence(B15F& drv);
void sendBits(const std::vector<std::bitset<4>>& buffer, B15F& drv);
std::bitset<4> convertBitset(const std::bitset<4>& input);