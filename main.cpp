#include "main.h"
#include <iostream>
#include <string>
#include <unistd.h> // For isatty

int main() {
    std::cout << "Starting" << std::endl;

    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0x0F);
    drv.setRegister(&PORTA, 0);

    // Check if input is coming from a pipe
    if (!isatty(fileno(stdin))) {
        // Piped input detected
        std::string inputString;
        std::getline(std::cin, inputString); // Read the entire line from stdin
        std::cout << "Input: " << inputString << std::endl;

        drv.setRegister(&DDRA, 0x0F);
        std::vector<std::bitset<4>> *buffer = sendData(drv, inputString); // Assuming sendData can take a string as parameter
        
        sendSequence(drv, STARTSYMBOL);
        sendBits((*buffer), drv); // Call the defined sendBits function
        sendSequence(drv, ENDTRANSMISSIONSYMBOL);
        drv.delay_ms(1000);
    } else {
        // No piped input, switch to receiving mode
        drv.setRegister(&DDRA, 0xF0);
        getData(drv);
    }

    return 0;
}
