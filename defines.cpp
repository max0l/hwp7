#include "defines.h"

const std::unordered_map<std::bitset<4>, std::bitset<2>> mappingTable = {
    {std::bitset<4>("0000"), std::bitset<2>("00")},
    {std::bitset<4>("0101"), std::bitset<2>("01")},
    {std::bitset<4>("0110"), std::bitset<2>("10")},
    {std::bitset<4>("0011"), std::bitset<2>("11")},
};