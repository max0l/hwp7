#include <iostream>
#include <b15f/b15f.h>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <bitset>

int main() {
    B15F& drv = B15F::getInstance();
    drv.setRegister (&DDRA , 0xF0);

    std::cout << "Starting" << std::endl;
    uint8_t tmp;

    //this program will read the contents of any file, and sends it over the B15F board to another B15F board.
    //I try using the Manchester Encoding.

    //First some testing:

    while(1) {
        //std::cout << "Schreibe 0-3" << std::endl;
        tmp = drv.getRegister(&PINA);
        std::cout << std::bitset<8>(tmp) << std::endl;
        tmp = 0;
        drv.delay_ms(200);
    }
    

}