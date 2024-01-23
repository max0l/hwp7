#pragma once
#include "defines.h"
#include "send.h"

void waitForStartBit(B15F& drv, uint8_t lanes);
bool getData(B15F& drv, uint8_t lanes);
void writeToBuffer(std::vector<std::bitset<4>> *buffer, B15F& drv, uint8_t lanes);
std::vector<std::bitset<4>>* cleanSonderzeichen(std::vector<std::bitset<4>> *buffer);
void processBuffer(std::vector<std::bitset<4>> *buffer);
void waitForACK(B15F& drv, uint8_t lanes);