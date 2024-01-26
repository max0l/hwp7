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
    drv.setRegister(&DDRA, 0x00);
    //Should set lanes to 0x0f if there is no input and to 0xf0 if there is input
    if(drv.getRegister(&PINA) == 0x00) {
        lanes = 0x0f;
        drv.setRegister(&DDRA, 0x0f);
        drv.setRegister(&PORTA, 0x0f);
        readOnly = false;
        waitForACK(drv, lanes);
    } else {
        drv.setRegister(&DDRA, 0xf0);
        lanes = 0xf0;
        readOnly = true;
    }

    while(readOnly) {
        if(drv.getRegister(&PINA) == 0x0f) {
            sendSequence(drv, ACKSYMBOL, lanes);
            readOnly = false;
        }
        
    }

    drv.setRegister(&PORTA, 0x00);

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
                std::cerr << "Got ACK!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            }
            //TODO: THIS CAN CAUSE ERRORS!
            if((uint8_t)sendingcounter==sendingBuffer->size()){
                isSending = false;
                sendSequence(drv, ENDTRANSMISSIONSYMBOL, lanes);
                if(!gotSomething) {
                    isReceiving = false;
                    std::cerr << "everything done. Exiting now" << std::endl;
                }
                
                continue;
            }
            //NOTE: I have to put every symbol in the buffer so everything is send automatically in the right order
            sendBits((*sendingBuffer), drv, lanes, sendingcounter);
            sendingcounter++;

        }
        if(gotSomething) {
            isReceiving = receiveBits(receivingBuffer, drv, lanes);
            if(!isReceiving) {
                gotSomething = false;
            }
            continue;
        }

        gotSomething = checkForStartSymbol(drv, lanes);

        //drv.delay_ms(1000);
        
    }

    drv.setRegister(&PORTA, 0x00);

    return 0;
}