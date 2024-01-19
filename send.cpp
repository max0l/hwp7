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
        std::cout << "No conversion needed" << std::endl;
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
    drv.setRegister(&PORTA, STARTSYMBOL);
    drv.delay_ms(BIT_PERIOD);
}

void sendBits(const std::vector<std::bitset<4>>& buffer, B15F& drv) {
    std::cout << "------------------Sening bits------------------" << std::endl;
    for (size_t i = 0; i < buffer.size(); i++) {
        //If the current bit is uqual to the next, send one CONTROL_BIT1
        std::cout << buffer[i] << std::endl;
        if((i<buffer.size()-1) && (buffer[i] == buffer[i+1])) {
            drv.setRegister(&PORTA, DOUBLESYMBOL);
            drv.delay_ms(BIT_PERIOD);
        }
        drv.setRegister(&PORTA, buffer[i].to_ulong());

        drv.delay_ms(BIT_PERIOD);
    }
}

