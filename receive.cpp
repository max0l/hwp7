#include "receive.h"

const std::unordered_map<std::bitset<4>, std::bitset<2>> mappingTable = {
    {std::bitset<4>(0b0000), std::bitset<2>(0b00)},
    {std::bitset<4>(0b0101), std::bitset<2>(0b01)},
    {std::bitset<4>(0b0110), std::bitset<2>(0b10)},
    {std::bitset<4>(0b0011), std::bitset<2>(0b11)},
};

/*
void waitForStartBit(B15F& drv, uint8_t lanes) {
    std::cerr << "------------------Wait for Start bit------------------" << std::endl;
    uint8_t shift;
    if(lanes == 0x0f) {
        shift = 0;
    } else {
        shift = 4;
    }
    //wait for start bit has been send 3 times in a row. Wait invinite time
    uint8_t current = drv.getRegister(&PINA) & lanes;
    while(1) {
        uint8_t tmp = drv.getRegister(&PINA) & lanes;
        if(tmp != current) {
            std::cerr << "Got Something new " <<std::bitset<8>(current) << std::endl;
            current = tmp;
        } else {
            //std::cerr << "Got Nothing new " <<std::bitset<8>(current) << std::endl;
            continue;
        }
        current &= 0b00001111;
        if(current == STARTSYMBOL) {
            std::cerr << "Got Start Bit" << std::endl;
            drv.setRegister(&DDRA, 0x0F);
            sendSequence(drv, ACKSYMBOL, lanes<<shift);
            drv.setRegister(&DDRA, 0x0F);
            return;
        }
    }
    std::cerr << "------------------exiting while loop startbits------------------" << std::endl;
}
*/
/*
bool getData(B15F& drv, uint8_t lanes) {
    std::cerr << "------------------Get Data------------------" << std::endl;
    std::vector<std::bitset<4>>* buffer = new std::vector<std::bitset<4>>;
    waitForStartBit(drv, lanes);
    writeToBuffer(buffer, drv, lanes);
    if (!buffer->empty()) {
        processBuffer(cleanSonderzeichen(buffer));
    }

    buffer->clear();
    return true;
}
*/
void writeToBuffer(std::vector<std::bitset<4>> *buffer, B15F& drv, uint8_t lanes) {
    //std::cerr << "------------------Write to Buffer------------------" << std::endl;
    //std::cerr << "Writing to Buffer" << std::endl;
    uint8_t shift;
    if(lanes == 0x0f) {
        shift = 0;
    } else {
        shift = 4;
    }

    uint8_t data = drv.getRegister(&PINA) & lanes;
    while (1) {
        uint8_t tmp = drv.getRegister(&PINA) & lanes;
        if(tmp != data) {
            //std::cerr <<std::bitset<4>(data) << std::endl;
            data = tmp;
        } else {
            //std::cerr << "Got Nothing new " <<std::bitset<8>(current) << std::endl;
            continue;
        }

        if(data>>shift == DOUBLESYMBOL) {
            //std::cerr << "Got Double Symbol" << std::endl;
            continue;
        }
        if(data>>shift == ENDTRANSMISSIONSYMBOL) {
            std::cerr << "Got End Transmission Symbol" << std::endl;
            return;
        }

        buffer->push_back(data>>shift);

        //data &= 0b00001111;
        /*
        switch(data) {
            case DOUBLESYMBOL:
                std::cerr << "Got Double Symbol" << std::endl;
                continue;
                break;
            case ENDTRANSMISSIONSYMBOL:
                return;
            default:
                buffer->push_back(data);
        }
        */
    }
    
}

std::vector<std::bitset<4>>* cleanSonderzeichen(std::vector<std::bitset<4>> *buffer) {
    
    /*
    std::cerr << "The buffer is:" << std::endl;
    for (size_t i = 0; i < buffer->size(); i++) {
        std::cerr << (*buffer)[i] << std::endl;
    }
    */
    //std::cerr << "------------------Cleaning Buffer------------------" << std::endl;
    std::vector<std::bitset<4>>* cleanedBuffer = new std::vector<std::bitset<4>>;
    for (size_t i = 0; i < buffer->size(); i++) {
        //std::cerr << "Checking if " << (*buffer)[i] << " = " << std::bitset<4>((*buffer)[i]) << " is a valid symbol" << std::endl;
        //print mappingTable
        /*<<<
        for (auto it = mappingTable.begin(); it != mappingTable.end(); ++it) {
            std::cerr << it->first << " => " << it->second << '\n';
        }
        */
        if(mappingTable.contains(std::bitset<4>((*buffer)[i]))) {
            cleanedBuffer->push_back((*buffer)[i]);
        } else if((*buffer)[i] == HASHSYMBOL) {
            //std::cerr << "Got Hash" << std::endl;
            break;
        }
        
        /*else {
            std::cerr << (*buffer)[i] << " is not a valid symbol"  << std::endl;
        }*/
    }
    /*
    std::cerr << "The cleaned buffer is:";
    if(cleanedBuffer->empty()) {
        std::cerr << "empty" << std::endl;
    } else {
        std::cerr << "not empty" << std::endl;
    }
    */
    /*
    for (size_t i = 0; i < cleanedBuffer->size(); i++) {
        std::cerr << (*cleanedBuffer)[i] << std::endl;
    }
    */
    return cleanedBuffer;
}

std::string processBuffer(std::vector<std::bitset<4>> *buffer) {
    //std::cerr << "------------------Processing Buffer------------------" << std::endl;
    std::vector<unsigned char> data;

    std::string dataString = "";

    unsigned char currentByte = 0; // Temporary byte to accumulate bits
    int bitCount = 0; // Count of bits in the currentByte

    for (const auto &bits : *buffer) {
        // Extracting the last 2 bits
        unsigned char lastTwoBits = bits.to_ulong() & 0b11;

        // Shifting and adding the last 2 bits to currentByte
        currentByte <<= 2;
        currentByte |= lastTwoBits;
        bitCount += 2;

        // When 8 bits are collected, add to data and reset for next byte
        if (bitCount == 8) {
            data.push_back(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }

    // Handling any remaining bits that didn't make a full byte
    if (bitCount > 0) {
        currentByte <<= (8 - bitCount); // Align remaining bits to the left
        data.push_back(currentByte);
    }
    
    // Print out all Bits in data bytewise
    /*
    std::cerr << "Data is: " << std::endl;
    for (size_t i = 0; i < data.size(); i++) {
        std::cerr << std::bitset<8>(data[i]) << std::endl;
    }
    */

    std::cerr << "Translated: " << std::endl;
    for (size_t i = 0; i < data.size(); i++) {
        //std::cout << data[i];
        dataString += data[i];
    }
    return dataString;
}

bool waitForSymbol(uint8_t symbol, uint8_t lanes, uint8_t current) {
    //std::cerr << "------------------Wait for ACK------------------" << std::endl;
    //TODO: REWORK BITMANIPULATION
    //drv.setRegister(&DDRA, 0x00);
    //wait for start bit has been send 3 times in a row. Wait invinite time
    uint8_t shift;
    if(lanes == 0x0f) {
        //Shift 4 to the right to read symbols;
        shift = 4;
    } else {
        //Shift 0 to the right to read symbols;
        shift = 0;
    }
    if((current&~lanes)>>shift == symbol) {
        //std::cerr << "------------------Reading: "<< std::bitset<4>(current>>shift) << " from board: " << std::bitset<8>(current) <<" ------------------" << std::endl;
        //std::cerr << std::bitset<8>(current) << std::endl;
        //std::cerr << "Got ACK" << std::endl;
        //std::cerr << "------------------exiting while loop ACK------------------" << std::endl;
        return true;
    } else {
        return false;
    }
    
    
}
/*

bool checkForStartSymbol(B15F& drv, uint8_t lanes, uint8_t current) {
    uint8_t shift;
    if(lanes == 0x0f) {
        shift = 4;
    } else {
        shift = 0;
    }
    //std::cerr << "Current is: " << std::bitset<8>(current) << std::endl;
    if((current&~lanes)>>shift == STARTSYMBOL) {
        std::cerr << "Got Start Bit" << std::endl;
        sendSequence(drv, ACKSYMBOL, lanes);
        std::cerr << "Sent ACK after STARTSYMBOL" << std::endl;
        //Maybe I need an OK
        return true;
    } else {
        //std::cerr << "No Sartsymbol" << std::endl;
        return false;
    }

}
*/

bool checkForSymbol(B15F& drv, uint8_t symbol , uint8_t lanes, uint8_t current) {
    uint8_t shift;
    if(lanes == 0x0f) {
        shift = 4;
    } else {
        shift = 0;
    }
    //std::cerr << "Current is: " << std::bitset<8>(current) << std::endl;
    if((current&~lanes)>>shift == symbol) {
        std::cerr << "Got Bit" << std::endl;
        sendSequence(drv, ACKSYMBOL, lanes);
        drv.delay_ms(BIT_PERIOD);
        std::cerr << "Sent ACK" << std::endl;
        //Maybe I need an OK
        return true;
    } else {
        //std::cerr << "No Sartsymbol" << std::endl;
        return false;
    }

} 
/*
uint8_t reverseBits(uint8_t input) {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        result |= ((input >> i) & 1) << (7 - i);
    }
    return result;
}
*/


bool receiveBits(std::vector<std::bitset<4>> *receivingBuffer, uint8_t lanes, uint8_t current, bool* dataWasCorrect) {
    //std::cerr << "------------------Receive Bits------------------" << std::endl;
    std::string dataString = "";
    uint8_t shift;
    if(lanes == 0x0f) {
        shift = 4;
    } else {
        shift = 0;
    }
    //TODO: Find cause why I have to set this again!
    uint8_t data = current >> shift;

    if(receivingBuffer->empty() || (*receivingBuffer)[receivingBuffer->size()-1] != data) {
        if(receivingBuffer->empty()) {
            std::cerr << "Buffer is empty, fill in first bits" << std::endl;
        }
        //std::cerr <<std::bitset<8>(data) << std::endl;
        receivingBuffer->push_back(data);
    }

    if(!receivingBuffer->empty() &&
        receivingBuffer->at(receivingBuffer->size()-1) == ENDTRANSMISSIONSYMBOL) {
        //std::cerr << "Got End Transmission Symbol" << std::endl;
        dataString = processBuffer(cleanSonderzeichen(receivingBuffer));
        std::cout << dataString << std::endl;
        (*dataWasCorrect) = checkHash(receivingBuffer, dataString);
        if((*dataWasCorrect)) {
            std::cerr << "Transmission was successfull" << std::endl;
        } else {
            std::cerr << "Transmission was not successfull" << std::endl;
        }
        return true;
    }
    return false;
}


bool checkHash (std::vector<std::bitset<4>> *receivingBuffer, std::string& data) {
    //scan for hashSymbol
    int hashIndex = 0;
    for(int i = 0; i < receivingBuffer->size(); i++) {
        if((*receivingBuffer)[i] == HASHSYMBOL) {
            std::cerr << "Found Hash" << std::endl;
            hashIndex = i;
            break;
        }
    }
    if(hashIndex == 0) {
        std::cerr << "No Hash found" << std::endl;
        return false;
    }
    //Copy Hash into new Buffer with HashIndex and vector insert
    std::vector<std::bitset<4>> *hashBuffer = new std::vector<std::bitset<4>>;
    hashBuffer->insert(hashBuffer->begin(), receivingBuffer->begin()+hashIndex+1, receivingBuffer->end());
    std::string hashString = processBuffer(cleanSonderzeichen(hashBuffer));

    std::string hashOfData = generareHash(data);

    if(hashString == hashOfData) {
        std::cerr << "Hashes are equal" << std::endl;
        return true;
    } else {
        std::cerr << "Hashes are not equal" << std::endl;
        return false;
    }

    
}