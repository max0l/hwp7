#include "main.h"
#include <iostream>
#include <string>
#include <unistd.h>

bool isSending = false;
bool isReceiving = true;
bool readOnly = false;

bool gotSomething = false;

uint8_t lanes = 0x0f;

int main(int argc, char* argv[]) {


    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--ro") {
            readOnly = true;
            break;
        }
    }

    // Rest of your code...

    std::cout << "Starting" << std::endl;

    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, lanes);
    drv.setRegister(&PORTA, 0);
    std::string inputString;
    std::vector<std::bitset<4>> *sendingBuffer = nullptr;
    int sendingcounter = 0;

    std::vector<std::bitset<4>> *receivingBuffer = new std::vector<std::bitset<4>>;




    if (!isatty(fileno(stdin))) {
        std::getline(std::cin, inputString); // Read the entire line from stdin
        std::cout << "Input: " << inputString << std::endl;
        sendingBuffer = generateBufferToSend(inputString);
        isSending = true;
    } else {
        std::cout << "No input" << std::endl;
        isSending = false;
    }


    while(isSending || isReceiving) {
        if(isSending) {
            if(sendingcounter==0){
                sendSequence(drv, STARTSYMBOL, lanes);
                waitForACK(drv, lanes);
            }
            //TODO: THIS CAN CAUSE ERRORS!
            if((uint8_t)sendingcounter==sendingBuffer->size()){
                isSending = true;
                sendSequence(drv, ENDTRANSMISSIONSYMBOL, lanes);
                continue;
            }
            //NOTE: I have to put every symbol in the buffer so everything is send automatically in the right order
            sendBits((*sendingBuffer), drv, lanes, sendingcounter);
            sendingcounter++;

        }
        if(gotSomething) {
            isReceiving = receiveBits(receivingBuffer, drv, ~lanes);
            if(!isReceiving) {
                gotSomething = false;
            }
            continue;
        }

        gotSomething = checkForStartSymbol(drv, ~lanes);

        //drv.delay_ms(1000);
    }


    return 0;
}