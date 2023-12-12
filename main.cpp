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
    std::getline(std::cin, input);

    std::vector<std::bitset<4>> buffer = splitIntoFourBits(input);
    while (1) {
        sendBits(buffer, drv); // Call the defined sendBits function
        drv.delay_ms(1000);
    }
    
}

std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input) {
    std::vector<std::bitset<4>> buffer;
    for (size_t i = 0; i < input.size(); i++) {
        std::bitset<4> bits(input[i]);
        buffer.push_back(std::bitset<4>(bits >> 4)); 
        buffer.push_back(std::bitset<4>(bits));
        std::cout << "bits: " << bits << std::endl;
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
        drv.setRegister(&PORTA, buffer[i].to_ulong());
        drv.delay_ms(BIT_PERIOD);
    }
}

void waitForStartBit(B15F& drv) {
    int i = 0;
    //wait for start bit has been send 3 times in a row. Wait invinite time
    while(1) {
        if(drv.getRegister(&PINA) == CONTROL_BIT1) {
            i++;
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
        processBuffer(buffer);
    } 
    
}

void writeToBuffer(std::vector<std::bitset<4>>& buffer, B15F& drv) {
    int controlCount = 0;
    while (1) {
        uint8_t data = drv.getRegister(&PINA) & 0b00001111;
        if(data== CONTROL_BIT1) {
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


void processBuffer(const std::vector<std::bitset<4>>& buffer) {
    std::vector<char> data;
    for (size_t i = 0; i < buffer.size(); i += 2) {
        char combinedBits;
        combinedBits |= (buffer[i].to_ulong() << 4);
        combinedBits |= buffer[i + 1].to_ulong();
        data.push_back(combinedBits);
    }
    
    std::cout << "Received: ";
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << data[i];
    }
}