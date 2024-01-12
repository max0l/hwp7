#include "send.h"
   
std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input) {
    std::cout << "------------------Split into four bites------------------" << std::endl;
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

void sendData(B15F& drv) {

    // Define the function sendStartSequence

    std::string input;
    std::cout << "Bitte geben Sie einen String ein: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::getline(std::cin, input);

    std::cout << "Ihre Eingabe lautet: " << input << std::endl;
    //print input as binary
    for (size_t i = 0; i < input.size(); i++) {
        std::cout << std::bitset<8>(input[i]) << std::endl;
    }
    std::cout << std::endl;


    std::vector<std::bitset<4>> buffer = splitIntoFourBits(input);

    while (1) {
        sendStartSequence(drv);
        sendBits(buffer, drv); // Call the defined sendBits function
        drv.delay_ms(1000);
    } 
}

void sendStartSequence(B15F& drv) {
    std::cout << "------------------Send Start Sequence------------------" << std::endl;
    //send 3x CONTROL_BIT1 to indicate start of transmission
    for(int i = 0; i<3; i++) {
        drv.setRegister(&PORTA, CONTROL_BIT1);
        std::cout << "Sending Start Sequence: Control bit1" << std::endl;
        drv.delay_ms(BIT_PERIOD);
        drv.setRegister(&PORTA, 0);
        std::cout << "Sending Start Sequence: 0" << std::endl;
        drv.delay_ms(BIT_PERIOD);

    }
}

void sendBits(const std::vector<std::bitset<4>>& buffer, B15F& drv) {
    std::cout << "------------------Sening bits------------------" << std::endl;
    for (size_t i = 0; i < buffer.size(); i++) {
        //If the current bit is uqual to the next, send one CONTROL_BIT1
        std::cout << buffer[i] << std::endl;
        if((i<buffer.size()-1) && (buffer[i] == buffer[i+1])) {
            drv.setRegister(&PORTA, CONTROL_BIT1);
            drv.delay_ms(BIT_PERIOD);
            drv.setRegister(&PORTA, buffer[i].to_ulong());
            drv.delay_ms(BIT_PERIOD);
            drv.setRegister(&PORTA, CONTROL_BIT1);
            drv.delay_ms(BIT_PERIOD);
            drv.setRegister(&PORTA, buffer[i].to_ulong());
            i+=1;
        }
        if(buffer[i] == CONTROL_BIT1) {
            //If the current bit is the same as the Controlbit bit, send CONTROL_BIT1 2 times
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

