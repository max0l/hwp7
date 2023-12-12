#include "main.h"

int main() {

    std::cout << "Starting" << std::endl;

    B15F& drv = B15F::getInstance();
    drv.setRegister (&DDRA , 0x0F);


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
            sendData(drv);
            // Hier könnte der Code zum Senden stehen
            break;
        case 1:
            getData(drv);
            // Hier könnte der Code zum Empfangen stehen
            break;
    }

    return 0;
}



void sendData(B15F& drv) {

    std::string input;
    std::cout << "Bitte geben Sie einen String ein: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::getline(std::cin, input);

    std::cout << "Ihre Eingabe lautet: " << input << std::endl;
    std::vector<std::bitset<4>> buffer = splitIntoFourBits(input);

    while (1) {
        sendBits(buffer, drv); // Call the defined sendBits function
        drv.delay_ms(1000);
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
    //send 3x CONTROL_BIT1 to indicate start of transmission
    for(int i = 0; i<3; i++) {
        drv.setRegister(&PORTA, CONTROL_BIT1);
        drv.delay_ms(BIT_PERIOD);
    }
    for (size_t i = 0; i < buffer.size(); i++) {
        //If the current bit is uquals to the next, send one CONTROL_BIT1

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
    //wait for start bit has been send 3 times in a row. Wait invinite time
    while(1) {
        uint8_t current = drv.getRegister(&PINA) & 0b00001111;
        if(current == CONTROL_BIT1) {
            i++;
        } else if(current == 0) {
            continue;
        } else {
            i = 0;
        }
        if(i == 3) {
            return;
        }
    }
}

void getData(B15F& drv) {
    std::vector<std::bitset<4>> buffer;
    while(1) {
        waitForStartBit(drv);
        writeToBuffer(buffer, drv);

        processBuffer(cleanBuffer(buffer));
    } 
    
}

void writeToBuffer(std::vector<std::bitset<4>>& buffer, B15F& drv) {
    int controlCount = 0;
    while (1) {
        uint8_t data = drv.getRegister(&PINA) & 0b00001111;
        if(data == CONTROL_BIT1 && buffer.size() > 0) {
            controlCount++;
            if(controlCount == 3) {
                for(int i = 0; i<3; i++) {
                    buffer.pop_back();
                }
                return;
            }
        } else {
            controlCount = 0;
            buffer.push_back(data);
        }
    }
    
    
}

std::vector<std::bitset<4>> cleanBuffer(const std::vector<std::bitset<4>>& buffer) {
    std::vector<std::bitset<4>> cleanBuffer;
    for (size_t i = 0; i < buffer.size(); i++) {
        if(buffer[i] == CONTROL_BIT1 && buffer[i+1] == CONTROL_BIT1 && i < buffer.size()-1) {
            cleanBuffer.push_back(CONTROL_BIT1);
            i++;
            continue;
        }
        if(buffer[i] == CONTROL_BIT1 && buffer[i+1] != CONTROL_BIT1 && i < buffer.size()-1) {
            cleanBuffer.push_back(buffer[i]);
            i++;
            continue;
        }
        cleanBuffer.push_back(buffer[i]);
    }
    return cleanBuffer;
}


void processBuffer(const std::vector<std::bitset<4>>& buffer) {
    std::vector<char> data;
    for (size_t i = 0; i < buffer.size(); i += 2) {
        char combinedBits = 0; // Initialize combinedBits
        combinedBits |= (buffer[i].to_ulong() << 4);
        combinedBits |= buffer[i + 1].to_ulong();
        data.push_back(combinedBits);
    }
    
    std::cout << "Received: ";
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << data[i];
    }
    std::cout << std::endl;
}