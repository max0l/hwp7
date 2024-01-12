#include "receive.h"

void waitForStartBit(B15F& drv) {
    std::cout << "------------------Wait for Start bit------------------" << std::endl;
    int i = 0;
    //wait for start bit has been send 3 times in a row. Wait invinite time
    uint8_t current = drv.getRegister(&PINA) & 0b00001111;
    while(1) {
        uint8_t tmp = drv.getRegister(&PINA) & 0b00001111;
        if(tmp != current) {
            std::cout << "Got Something new " <<std::bitset<8>(current) << std::endl;
            current = tmp;
        } else {
            //std::cout << "Got Nothing new " <<std::bitset<8>(current) << std::endl;
            continue;
        }
        current &= 0b00001111;
        if(current == CONTROL_BIT1) {
            std::cout << "Got Start Bit" << std::endl;
            i++;
        } else if(current == 0) {
            std::cout << "Got 0" << std::endl;
            continue;
        } else {
            i = 0;
        }
        if(i == 3) {
            std::cout << "Got 3 Start Bits" << std::endl;
            return;
        }
        std::cout << std::endl;
    }
    std::cout << "------------------exiting while loop startbits------------------" << std::endl;
}
void getData(B15F& drv) {
    std::cout << "------------------Get Data------------------" << std::endl;
    std::vector<std::bitset<4>>* buffer = new std::vector<std::bitset<4>>;
    while(1) {
        waitForStartBit(drv);
        writeToBuffer(buffer, drv);
        if (!buffer->empty()) {
            processBuffer(cleanBuffer(buffer));
        }

        buffer->clear();
    } 
}
void writeToBuffer(std::vector<std::bitset<4>> *buffer, B15F& drv) {
    std::cout << "------------------Write to Buffer------------------" << std::endl;
    int controlCount = 0;
    std::cout << "Writing to Buffer" << std::endl;
    uint8_t data = drv.getRegister(&PINA) & 0b00001111;
    while (1) {
        uint8_t tmp = drv.getRegister(&PINA) & 0b00001111;
        if(tmp != data) {
            std::cout << "Reveived: " <<std::bitset<8>(data) << std::endl;
            data = tmp;
        } else {
            //std::cout << "Got Nothing new " <<std::bitset<8>(current) << std::endl;
            continue;
        }
        //data &= 0b00001111;
        if(data == CONTROL_BIT1 || data == 0) {
            std::cout << "Got Control Bit" << std::endl;
            if(data == CONTROL_BIT1) {
                buffer->push_back(data);
                controlCount++;
            }
            if(data == 0) {
                buffer->push_back(data);
            }
            if(controlCount == 3) {
                for(int i = 0; i<5; i++) {
                    buffer->pop_back();
                }
                return;
            }
        } else {
            controlCount = 0;
            buffer->push_back(data);
        }
    }  
    
}

std::vector<std::bitset<4>>* cleanBuffer(std::vector<std::bitset<4>> *buffer) {
    std::cout << "The buffer is:" << std::endl;
    for (size_t i = 0; i < buffer->size(); i++) {
        std::cout << (*buffer)[i] << std::endl;
    }
    std::cout << "------------------Cleaning Buffer------------------" << std::endl;
    std::vector<std::bitset<4>>* cleanedBuffer = new std::vector<std::bitset<4>>;
    for (size_t i = 0; i < buffer->size(); i++) {
        if((*buffer)[i] == CONTROL_BIT1 && (*buffer)[i+1] == CONTROL_BIT1 && i < buffer->size()-1) {
            cleanedBuffer->push_back(CONTROL_BIT1);
            i++;
            continue;
        }
        if((*buffer)[i] == CONTROL_BIT1 && (*buffer)[i+1] != CONTROL_BIT1 && i < buffer->size()-1) {
            cleanedBuffer->push_back((*buffer)[i]);
            i++;
            continue;
        }
        cleanedBuffer->push_back((*buffer)[i]);
    }
    return cleanedBuffer;
}

void processBuffer(std::vector<std::bitset<4>> *buffer) {
    std::cout << "------------------Processing Buffer------------------" << std::endl;
    std::vector<unsigned char> data;

    // Fehlerbehubung: Wenn ungerade anzahl an bits
    for (size_t i = 1; i < buffer->size()-1; i += 2) {
        unsigned char combinedBits = 0; // Initialize combinedBits
        combinedBits |= ((*buffer)[i].to_ulong() << 4);
        combinedBits |= (*buffer)[i + 1].to_ulong();
        data.push_back(combinedBits);
    }
    
    // Print out all Bits in buffer bytewise (in buffer they are 4 bits)
    std::cout << "Received: " << std::endl;
    for (size_t i = 1; i < buffer->size(); i += 2) {
        std::cout << (*buffer)[i] << (*buffer)[i + 1] << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Translated: " << std::endl;
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << data[i];
    }
    std::cout << std::endl;
}
