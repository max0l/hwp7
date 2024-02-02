#pragma once

#include <iostream>
#include <b15f/b15f.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <bitset>
#include <unordered_map>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/md5.h>


#define BIT_PERIOD 50
//#define CONTROL_BIT1 0b0001
#define PACKET_SIZE 8
#define STARTSYMBOL 0b1000
#define STOPSYMBOL 0b01110
#define ENDTRANSMISSIONSYMBOL 0b1111
#define PACKETNUMBERSYMBOL 0b1001
#define HASHSYMBOL 0b1100
#define ACKSYMBOL 0b1011
#define DOUBLESYMBOL 0b1010
#define ERRORSYMBOL 0b1101

//const std::unordered_map<std::bitset<4>, std::bitset<2>> mappingTable;
