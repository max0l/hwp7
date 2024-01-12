#include "main.h"

int main() {

    std::cout << "Starting" << std::endl;

    B15F& drv = B15F::getInstance();

    int eingabe;

    std::cout << "Möchten Sie senden oder empfangen? Geben Sie 0 zum Senden, 1 zum Empfangen ein: ";
    std::cin >> eingabe;

    drv.setRegister (&DDRA , 0xFF);
    drv.setRegister (&PORTA , 0);

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
