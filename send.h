#pragma once
#include "defines.h"

std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input);
std::vector<std::bitset<4>>* generateBufferToSend(std::string input);
void sendSequence(B15F& drv, u_int8_t sequence, uint8_t lanes);
void sendBits(const std::vector<std::bitset<4>>& buffer, B15F& drv, uint8_t lanes, int count);
std::vector<std::bitset<4>> insertDoubleSymbol(const std::vector<std::bitset<4>>& buffer);