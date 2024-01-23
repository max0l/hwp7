#include "send.h"
   
std::bitset<4> convertBitset(const std::bitset<2>& input) {
    std::cout << "Converting bitset: " << input << std::endl;
    if (input == 0b00) {
        std::cout << "Converted to: 0b0000" << std::endl;
        return 0b0000;
    } else if (input == 0b11) {
        std::cout << "Converted to: 0b0011" << std::endl;
        return 0b0011;
    } else if (input == 0b01) {
        std::cout << "Converted to: 0b0101" << std::endl;
        return 0b0101;
    } else if (input == 0b10) {
        std::cout << "Converted to: 0b0110" << std::endl;
        return 0b0110;
    } else {
        // Handle other cases if needed
        std::cout << "Something went wrong" << std::endl;
        return input.to_ulong(); // Convert the remaining bits to an integer
    }
}

std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input) {
    std::cout << "------------------Split into four bits------------------" << std::endl;
    std::vector<std::bitset<4>> buffer;

    for (size_t i = 0; i < input.size(); i++) {
        std::bitset<8> inputBits(input[i]);
        for (int j = 6; j >= 0; j -= 2) { // Corrected loop condition
            std::bitset<2> twoBits((inputBits >> j).to_ulong()); // Convert to an integer before shifting
            std::bitset<4> convertedBits = convertBitset(twoBits);
            buffer.push_back(convertedBits);
        }
    }

    return buffer;
}

void sendData(B15F& drv, std::string input) {


    std::vector<std::bitset<4>> buffer = splitIntoFourBits(input);

    while (1) {
        sendSequence(drv, STARTSYMBOL);
        sendBits(buffer, drv); // Call the defined sendBits function
        sendSequence(drv, ENDTRANSMISSIONSYMBOL);
        drv.delay_ms(1000);
    } 
}

void sendSequence(B15F& drv, u_int8_t sequence) {
    std::cout << "------------------Send Sequence: "<< sequence <<" ------------------" << std::endl;
    drv.setRegister(&PORTA, sequence);
    drv.delay_ms(BIT_PERIOD);
}

void sendBits(const std::vector<std::bitset<4>>& buffer, B15F& drv) {
    std::cout << "------------------Sening bits------------------" << std::endl;
    for (size_t i = 0; i < buffer.size(); i++) {
        //If the current bit is uqual to the next, send one DOUBLESYMBOL
        std::cout << buffer[i] << std::endl;
        drv.setRegister(&PORTA, buffer[i].to_ulong());

        drv.delay_ms(BIT_PERIOD);
        if((i<buffer.size()-1) && (buffer[i] == buffer[i+1])) {
            std::cout << std::bitset<4>(DOUBLESYMBOL) << std::endl;
            drv.setRegister(&PORTA, std::bitset<4>(DOUBLESYMBOL).to_ulong());
            drv.delay_ms(BIT_PERIOD);
        }

    }
}

