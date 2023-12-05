#include <iostream>
#include <b15f/b15f.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <bitset>

int main() {
    B15F& drv = B15F::getInstance();
    drv.setRegister (&DDRA , 0x0F);

    std::cout << "Starting" << std::endl;
    //uint8_t tmp;

    //this program will read the contents of any file, and sends it over the B15F board to another B15F board.
    //I try using the Manchester Encoding.

    //First some testing:

    while(1) {
        std::cout << "Schreibe 0-3" << std::endl;
        for(uint8_t i = 0; i<16;i++) {
            drv.setRegister(&PORTA, i);
            std::cout << std::bitset<8>(drv.getRegister(&PINA)) << "=" << (int) i << "="<< std::bitset<8>(i) << std::endl;
            drv.delay_ms(500);
        }
        //tmp = 0;
        drv.delay_ms(500);
    }
    

}