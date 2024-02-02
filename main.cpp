#include "main.h"
#include <iostream>
#include <string>
#include <unistd.h>

bool isSending = false;
bool isReceiving = true;
bool readOnly = false;

bool gotSomething = false;

bool* dataWasCorrect = new bool(false);
bool transmissionEnded = false;

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
            gotNoACK = !waitForSymbol(ACKSYMBOL, lanes, drv.getRegister(&PINA));
        }
    } else {
        std::cerr << "Setting up lanes: I'm On 0xf0" << std::endl;
        drv.setRegister(&DDRA, 0xf0);
        lanes = 0xf0;
        sendSequence(drv, ACKSYMBOL, lanes);
        readOnly = true;
    }

    std::cerr << "Setting up lanes done! My lane: " << std::bitset<8>(lanes)<< std::endl;
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
                std::cerr << "Sending Start Symbol" << std::endl;
                sendSequence(drv, STARTSYMBOL, lanes);
                drv.delay_ms(BIT_PERIOD);
                if(waitForSymbol(ACKSYMBOL, lanes, drv.getRegister(&PINA))) {
                    std::cerr << "Got ACK! Sending Enabled" << std::endl;
                    sendingStarted = true;
                } else {
                    std::cerr << "no ack, retrying" << std::endl;
                    sendingStarted = false;
                }
            }
            //TODO: THIS CAN CAUSE ERRORS! (Casting a bigger int to uint8_t)
            if(sendingcounter==sendingBuffer->size()){
                
                std::cerr << "Sending done" << std::endl;

                uint8_t current = drv.getRegister(&PINA);
                
                sendingStarted = false;
                if(waitForSymbol(ACKSYMBOL, lanes, current)) {
                    sendingcounter = 0;
                    isSending = false;
                    std::cerr << "Ending Transmission!" << std::endl;
                } else if(waitForSymbol(ERRORSYMBOL, lanes, current)) {
                    sendingcounter = 0;
                    std::cerr << "Error while sending!" << std::endl;
                }
            }
            //NOTE: I have to put every symbol in the buffer so everything is send automatically in the right order
            if(sendingStarted) {
                std::cerr << "Sending: " << sendingcounter << " = " << (*sendingBuffer)[sendingcounter] << std::endl;
                sendBits((*sendingBuffer), drv, lanes, sendingcounter);

                sendingcounter++;
                if(sendingcounter == sendingBuffer->size()) {
                    std::cerr << "Sending End Transmission Symbol" << std::endl;
                    sendSequence(drv, ENDTRANSMISSIONSYMBOL, lanes);
                }
            }

        }

        if(!gotSomething && !transmissionEnded){
            //std::cerr << "Checking for start symbol" << std::endl;
            gotSomething = checkForSymbol(drv, STARTSYMBOL, lanes, drv.getRegister(&PINA));
            gotSomething ? std::cerr << "GOT SOMETHING. READING ENABLED" << std::endl : std::cerr << "No start symbol" << std::endl;
            if(gotSomething) {
                isReceiving = true;
            } else {
                isReceiving = false;
            }
        } else {
            //std::cerr << "Got something" << std::endl;
            transmissionEnded = receiveBits(receivingBuffer, lanes, drv.getRegister(&PINA), dataWasCorrect);          
            if(!isReceiving) {
                gotSomething = false;
            }

        }
        /*
        For next Time:
        - Die checks sind falsch, weil else if true wird und das zu fehlern führt
        */
        if(!(*dataWasCorrect) && transmissionEnded) {
            sendSequence(drv, ERRORSYMBOL, lanes);
            receivingBuffer->clear();
            std::cerr << "Sending: Data was not correct" << std::endl;
            isReceiving = true;
            gotSomething = false;
        } else if((*dataWasCorrect) && transmissionEnded) {
            std::cerr << "Sending: Data was correct" << std::endl;
            sendSequence(drv, ACKSYMBOL, lanes);
            isReceiving = false;
            gotSomething = false;
        }

        //std::cerr << "loop is looping" << std::endl;
        //drv.delay_ms(1000);
        
    }

    drv.setRegister(&PORTA, 0x00);

    return 0;
}