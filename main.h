#include <iostream>
#include <b15f/b15f.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <bitset>

#define BIT_PERIOD 1000
#define CONTROL_BIT1 0b0001

uint8_t readData(B15F& drv);
uint8_t interpretData(uint8_t &firstData, uint8_t &secondData);
std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input);
void sendData(B15F& drv);
void getData(B15F& drv);
void processBuffer(const std::vector<std::bitset<4>>& buffer);
void writeToBuffer(std::vector<std::bitset<4>>& buffer, B15F& drv);
