#include "main.h"

int main() {

    std::cout << "Starting" << std::endl;

    B15F& drv = B15F::getInstance();

    int eingabe;

    std::cout << "Möchten Sie senden oder empfangen? Geben Sie 0 zum Senden, 1 zum Empfangen ein: ";
    std::cin >> eingabe;

    while (std::cin.fail() || (eingabe != 0 && eingabe != 1)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ungültige Eingabe. Bitte geben Sie 0 zum Senden, 1 zum Empfangen ein: ";
        std::cin >> eingabe;
    }

    switch (eingabe) {
        case 0:
            drv.setRegister (&DDRA , 0xFF);
            sendData(drv);
            // Hier könnte der Code zum Senden stehen
            break;
        case 1:
            drv.setRegister (&DDRA , 0x00);
            getData(drv);
            // Hier könnte der Code zum Empfangen stehen
            break;
    }

    return 0;
}



void sendData(B15F& drv) {

     // Define the function sendStartSequence

    std::string input;
    std::cout << "Bitte geben Sie einen String ein: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::getline(std::cin, input);

    std::cout << "Ihre Eingabe lautet: " << input << std::endl;
    std::vector<std::bitset<4>> buffer = splitIntoFourBits(input);

    while (1) {
        sendStartSequence(drv);
        sendBits(buffer, drv); // Call the defined sendBits function
        drv.delay_ms(1000);
    }

    
}

void sendStartSequence(B15F& drv) {
    //send 3x CONTROL_BIT1 to indicate start of transmission
    for(int i = 0; i<3; i++) {
        drv.setRegister(&PORTA, CONTROL_BIT1);
        drv.delay_ms(BIT_PERIOD);
        drv.setRegister(&PORTA, 0);
        drv.delay_ms(BIT_PERIOD);
    }
}

std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input) {
    std::vector<std::bitset<4>> buffer;
    //std::cout << "Splitted Bits:" << std::endl;
    for (size_t i = 0; i < input.size(); i++) {
        std::bitset<8> bits(input[i]);
        buffer.push_back(std::bitset<4>(bits.to_ulong() >> 4)); 
        buffer.push_back(std::bitset<4>(bits.to_ulong()));
        //std::cout << std::bitset<4>(bits.to_ulong() >> 4) << std::bitset<4>(bits.to_ulong()) << std::endl;
    }
    
    return buffer;
}

void sendBits(const std::vector<std::bitset<4>>& buffer, B15F& drv) {
    for (size_t i = 0; i < buffer.size(); i++) {
        //If the current bit is uquals to the next, send one CONTROL_BIT1
        std::cout << buffer[i] << std::endl;
        if((i<buffer.size()-1) && (buffer[i] == buffer[i+1])) {
            drv.setRegister(&PORTA, CONTROL_BIT1);
            drv.delay_ms(BIT_PERIOD);
            drv.setRegister(&PORTA, buffer[i].to_ulong());
            drv.setRegister(&PORTA, CONTROL_BIT1);
            drv.delay_ms(BIT_PERIOD);
            drv.setRegister(&PORTA, buffer[i].to_ulong());
            i+=1;
        }
        if(buffer[i] == CONTROL_BIT1) {
            //If the current bit is the same as the next bit, send CONTROL_BIT1 2 times
            drv.setRegister(&PORTA, CONTROL_BIT1);
            drv.delay_ms(BIT_PERIOD);
            drv.setRegister(&PORTA, buffer[i].to_ulong());
        } else {
            //Send the bits normal esle wise
            drv.setRegister(&PORTA, buffer[i].to_ulong());
        }

        drv.delay_ms(BIT_PERIOD);
    }
}

void waitForStartBit(B15F& drv) {
    int i = 0;
    std::cout << "Waiting for start bit" << std::endl;
    //wait for start bit has been send 3 times in a row. Wait invinite time
    uint8_t current = drv.getRegister(&PINA);
    while(1) {
        uint8_t tmp = drv.getRegister(&PINA);
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
    }
}

void getData(B15F& drv) {
    std::vector<std::bitset<4>>* buffer = new std::vector<std::bitset<4>>;
    while(1) {
        waitForStartBit(drv);
        writeToBuffer(buffer, drv);
        if (!buffer->empty()) {
            processBuffer(cleanBuffer(buffer));
        }
    } 
}

void writeToBuffer(std::vector<std::bitset<4>> *buffer, B15F& drv) {
    int controlCount = 0;
    std::cout << "Writing to Buffer" << std::endl;
    uint8_t data = drv.getRegister(&PINA);
    while (1) {
        uint8_t tmp = drv.getRegister(&PINA);
        if(tmp != data) {
            std::cout << "Got Something new " <<std::bitset<8>(data) << std::endl;
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
    std::vector<char> data;
    for (size_t i = 0; i < buffer->size(); i += 2) {
        char combinedBits = 0; // Initialize combinedBits
        combinedBits |= ((*buffer)[i].to_ulong() << 4);
        combinedBits |= (*buffer)[i + 1].to_ulong();
        data.push_back(combinedBits);
    }
    
    std::cout << "Received: ";
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << data[i];
    }
    std::cout << std::endl;
}