#pragma once
#include "defines.h"
#include "send.h"

//void waitForStartBit(B15F& drv, uint8_t lanes);
//bool getData(B15F& drv, uint8_t lanes);
void writeToBuffer(std::vector<std::bitset<4>> *buffer, B15F& drv, uint8_t lanes);
std::vector<std::bitset<4>>* cleanSonderzeichen(std::vector<std::bitset<4>> *buffer);
void processBuffer(std::vector<std::bitset<4>> *buffer);
bool waitForACK(B15F& drv, uint8_t lanes, uint8_t current);
bool checkForStartSymbol(B15F& drv, uint8_t lanes, uint8_t current);
bool receiveBits(std::vector<std::bitset<4>> *receivingBuffer, B15F& drv, uint8_t lanes, uint8_t current);
//uint8_t reverseBits(uint8_t input);