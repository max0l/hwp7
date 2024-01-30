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
        if (std::string(argv[i]) == "--help") {
            std::cerr << "Ich brauche auch Hilfe" << std::endl;
            return 0;
        }
        if (std::string(argv[i]) == "--clear") {
            B15F& drv = B15F::getInstance();
            drv.setRegister(&DDRA, 0x00);
            return 0;
        }
    }

    // Rest of your code...

    std::cerr << "Starting" << std::endl;

    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0x00);
    //Should set lanes to 0x0f if there is no input and to 0xf0 if there is input
    if(drv.getRegister(&PINA) == 0x00) {
        std::cerr << "Setting up lanes: I'm On 0x0f" << std::endl;
        lanes = 0x0f;
        drv.setRegister(&DDRA, 0x0f);
        drv.setRegister(&PORTA, 0x0f);
        readOnly = false;
        bool gotNoACK = true;
        std::cerr << "Waiting for ACK" << std::endl;
        while(gotNoACK){
            gotNoACK = !waitForACK(lanes, drv.getRegister(&PINA));
        }
    } else {
        std::cerr << "Setting up lanes: I'm On 0xf0" << std::endl;
        drv.setRegister(&DDRA, 0xf0);
        lanes = 0xf0;
        sendSequence(drv, ACKSYMBOL, lanes);
        readOnly = true;
    }

    std::cerr << "Setting up lanes done!!!!!!!!!!!!!!!!!!!!! My lane: " << std::bitset<8>(lanes)<< std::endl;
    std::string inputString;
    std::vector<std::bitset<4>> *sendingBuffer = nullptr;
    int sendingcounter = 0;

    std::vector<std::bitset<4>> *receivingBuffer = new std::vector<std::bitset<4>>;




    if (!isatty(fileno(stdin))) {
        std::string line;
        while (std::getline(std::cin, line)) {
            //if its the last line, dont add a newline
            inputString += line + "\n"; // Append each line to the existing inputString
        }
        std::cerr << "Input: " << inputString << std::endl;
        sendingBuffer = generateBufferToSend(inputString);
        std::cerr << "Buffersize: " << sendingBuffer->size() << std::endl;
        isSending = true;
    } else {
        std::cerr << "No input" << std::endl;
        isSending = false;
    }

    drv.setRegister(&PORTA, 0x00);

    bool sendingStarted = false;
    
    std::cerr << "Starting loop" << std::endl;
    drv.delay_ms(BIT_PERIOD);
    while(isSending || isReceiving) {

        if(isSending) {
            if(sendingcounter==0){
                sendSequence(drv, STARTSYMBOL, lanes);
                drv.delay_ms(BIT_PERIOD);
                if(waitForACK(lanes, drv.getRegister(&PINA))) {
                    std::cerr << "Got ACK!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    sendingStarted = true;
                } else {
                    std::cerr << "no ack" << std::endl;
                    sendingStarted = false;
                }
            }
            //TODO: THIS CAN CAUSE ERRORS! (Casting a bigger int to uint8_t)
            if(sendingcounter==sendingBuffer->size()){
                isSending = false;
                std::cerr << "Sending done" << std::endl;
                sendSequence(drv, ENDTRANSMISSIONSYMBOL, lanes);
                sendingStarted = false;
            }
            //NOTE: I have to put every symbol in the buffer so everything is send automatically in the right order
            if(sendingStarted) {
                std::cerr << "Sending: " << sendingcounter << " = " << (*sendingBuffer)[sendingcounter] << std::endl;
                sendBits((*sendingBuffer), drv, lanes, sendingcounter);

                sendingcounter++;
            }

        }

        if(!gotSomething){
            std::cerr << "Checking for start symbol" << std::endl;
            gotSomething = checkForStartSymbol(drv, lanes, drv.getRegister(&PINA));
            gotSomething ? std::cerr << "GOT SOMETHING. READING ENABLED" << std::endl : std::cerr << "No start symbol" << std::endl;
            if(gotSomething) {
                isReceiving = true;
            } else {
                isReceiving = false;
            }
        }

        if(gotSomething) {
            //std::cerr << "Got something" << std::endl;
            isReceiving = receiveBits(receivingBuffer, lanes, drv.getRegister(&PINA));
            if(!isReceiving) {
                gotSomething = false;
            }
        }




        //std::cerr << "loop is looping" << std::endl;
        //drv.delay_ms(1000);
        
    }

    drv.setRegister(&PORTA, 0x00);

    return 0;
}