#include <iostream>
#include <b15f/b15f.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <bitset>

#define BIT_PERIOD 100

void sendDataWithManchester(uint8_t &data, B15F& drv);
uint8_t readData(B15F& drv);
uint8_t interpretData(uint8_t &firstData, uint8_t &secondData);

int main() {
    B15F& drv = B15F::getInstance();
    drv.setRegister (&DDRA , 0x00);

    std::cout << "Starting" << std::endl;
    //uint8_t tmp;

    //this program will read the contents of any file, and sends it over the B15F board to another B15F board.
    //I try using the Manchester Encoding.

    //First some testing:

    while(1) {
        auto data = readData(drv); 
        //std::cout <<"GOT: " <<std::bitset<8>(data) << "==" << (int) data << std::endl;
        uint8_t tmp1 = 0;
        uint8_t tmp2 = 0;
        
        drv.delay_ms(100);
    }
    

}


void sendDataWithManchester(uint8_t &data, B15F& drv) {
    drv.setRegister(&PORTA, (0x0F) & data);
    //drv.delay_us(10);
    drv.delay_ms(BIT_PERIOD/2);
    drv.setRegister(&PORTA, ((0x0F) & ~data));
    drv.delay_ms(BIT_PERIOD/2);

    drv.setRegister(&PORTA, (data>>4));
    //drv.delay_us(10);
    drv.delay_ms(BIT_PERIOD/2);
    drv.setRegister(&PORTA, (~data>>4));
    drv.delay_ms(BIT_PERIOD/2);
}


//This will get only 4 bit at a time
uint8_t readData(B15F& drv) {
    uint8_t firstData = drv.getRegister(&PINA);
    //std::cout << std::bitset<8>(firstData) << "==" << (int) firstData << std::endl;
    uint8_t secondData;
    bool hasChanged = false;
    while(!hasChanged) {
        //Doing this in manchester, means that if I reveived the firstData correctly, the secondData has to be the negated version of the first one
        secondData = drv.getRegister(&PINA);
        //std::cout << std::bitset<8>(secondData) << "==" << (int) secondData << std::endl;
        if(firstData != secondData) {
            if(firstData == ~(0xF0 & secondData) || 1) {
                //std::cout << "Data received correctly" << std::endl;
                hasChanged = true;
            } else {
                std::cout << "Data received incorrectly" << std::endl;
            }
            
        }
    }
    //std::cout << firstData << " " << secondData << std::endl;

    return interpretData(firstData, secondData);

}

uint8_t interpretData(uint8_t &firstData, uint8_t &secondData) {
    uint8_t solution = 0;
    for(uint8_t i = 0; i<4; i++) {
        if(((firstData >> i) & 0x01) == 1 && ((secondData >> i) & 0x01) == 0) {
            solution |= (1 << i);
        } else {
            solution &= ~(1 << i);
        }
    }
    return solution;
}

//Instead of sending each bit alone: send 2 4Bit that are together encoded in Manchester
//For example:
/*
0101
=
(1.)0101
(2.)1010

It will read for    PINA0: 10 -> 1
                    PINA1: 01 -> 0
*/