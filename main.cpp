#include "main.h"
#include <iostream>
#include <string>
#include <unistd.h>


bool position = false;
bool sendingDone = false;
bool receivingDone = false;


int main() {
    std::cout << "Starting" << std::endl;

    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0x0F);
    drv.setRegister(&PORTA, 0);
    std::string inputString;
    std::vector<std::bitset<4>> *buffer = nullptr;
    int sendingcounter = 0;



    if (!isatty(fileno(stdin))) {
        std::getline(std::cin, inputString); // Read the entire line from stdin
        std::cout << "Input: " << inputString << std::endl;
        buffer = generateBufferToSend(inputString);
    } else {
        std::cout << "No input" << std::endl;
        sendingDone = true;
    }


    while(!sendingDone && !receivingDone) {
        if(!sendingDone) {
            if(sendingcounter==0){
                sendSequence(drv, STARTSYMBOL, getLanes());
                //waitForACK(drv, getLanes());
            }
            if(sendingcounter==buffer->size()){
                sendingDone = true;
                sendSequence(drv, ENDTRANSMISSIONSYMBOL, getLanes());
                continue;
            }
            //NOTE: I have to put every symbol in the buffer so everything is send automatically in the right order
            sendBits((*buffer), drv, getLanes(), sendingcounter);
            sendingcounter++;

        }
        if(!receivingDone) {
            //wait for startsymbol
            //getData(drv, getLanes());


        }
        //drv.delay_ms(1000);
    }


    return 0;
}

uint8_t getLanes() {
    if(!position) {
        return 0b00001111;
    } else {
        return 0b11110000;
    }
}