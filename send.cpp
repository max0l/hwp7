#include "send.h"
   
std::bitset<4> convertBitset(const std::bitset<2>& input) {
    //std::cerr << "Converting bitset: " << input << std::endl;
    if (input == 0b00) {
        //std::cerr << "Converted to: 0b0000" << std::endl;
        return 0b0000;
    } else if (input == 0b11) {
        //std::cerr << "Converted to: 0b0011" << std::endl;
        return 0b0011;
    } else if (input == 0b01) {
        //std::cerr << "Converted to: 0b0101" << std::endl;
        return 0b0101;
    } else if (input == 0b10) {
        //std::cerr << "Converted to: 0b0110" << std::endl;
        return 0b0110;
    } else {
        // Handle other cases if needed
        std::cerr << "Something went wrong" << std::endl;
        return input.to_ulong(); // Convert the remaining bits to an integer
    }
}

std::vector<std::bitset<4>> splitIntoFourBits(const std::string& input) {
    //std::cerr << "------------------Split into four bits------------------" << std::endl;
    std::vector<std::bitset<4>> buffer;
    //std::cerr << "Converted Bits: "<< std::endl;
    for (size_t i = 0; i < input.size(); i++) {
        std::bitset<8> inputBits(input[i]);
        for (int j = 6; j >= 0; j -= 2) { // Corrected loop condition
            std::bitset<2> twoBits((inputBits >> j).to_ulong()); // Convert to an integer before shifting
            std::bitset<4> convertedBits = convertBitset(twoBits);
            //std::cerr << convertedBits << std::endl;
            buffer.push_back(convertedBits);
        }
    }

    return buffer;
}

std::vector<std::bitset<4>> insertDoubleSymbol(const std::vector<std::bitset<4>>& buffer) {
    std::vector<std::bitset<4>> modifiedBuffer;

    for (size_t i = 0; i < buffer.size(); i++) {
        modifiedBuffer.push_back(buffer[i]);

        if (i < buffer.size() - 1 && buffer[i] == buffer[i + 1]) {
            modifiedBuffer.push_back(DOUBLESYMBOL);
        }
    }

    return modifiedBuffer;
}

std::vector<std::bitset<4>>* generateBufferToSend(std::string input) {


    std::vector<std::bitset<4>> *buffer = new std::vector<std::bitset<4>>;
    (*buffer) = insertDoubleSymbol(splitIntoFourBits(input));
    (*buffer).push_back(HASHSYMBOL);
     std::vector<std::bitset<4>> *hashBuffer = generateHashBufferOfInput(input);
    (*buffer).insert((*buffer).end(), (*hashBuffer).begin(), (*hashBuffer).end());
    return buffer;
}

void sendSequence(B15F& drv, u_int8_t sequence, uint8_t lanes) {
    //If writing, shift is reversed then at receiving
    uint8_t shift;
    if(lanes == 0x0f) {
        shift = 0;
    } else {
        shift = 4;
    }
    //std::cerr << "Send Sequence: "<< std::bitset<4>(sequence) << " to board: " << std::bitset<8>(sequence << shift) << std::endl;
    drv.setRegister(&PORTA, sequence << shift);
    drv.delay_ms(BIT_PERIOD);
}

void sendBits(const std::vector<std::bitset<4>>& buffer, B15F& drv, uint8_t lanes, int count) {
    //std::cerr << "Sending bits: " << buffer[count]<< " On Lanes: " << std::bitset<8>(lanes) << std::endl;
    sendSequence(drv, buffer[count].to_ulong(), lanes);
    drv.delay_ms(BIT_PERIOD);
}

std::vector<std::bitset<4>>* generateHashBufferOfInput(std::string input) {


    std::vector<std::bitset<4>> *buffer = new std::vector<std::bitset<4>>;
    (*buffer) = insertDoubleSymbol(splitIntoFourBits(generareHash(input)));	
    return buffer;
}
/*
std::string generareHash(std::string input) {
    std::string digest;
    CryptoPP::SHA1 hash;

    CryptoPP::StringSource foo(input, true,
    new CryptoPP::HashFilter(hash,
      new CryptoPP::Base64Encoder (
         new CryptoPP::StringSink(digest))));


    std::cerr << "Hash: " << digest << std::endl;
    return digest;
}
*/

std::string generareHash(const std::string& input) {
    // Create a new instance of the MD5 hash function
    CryptoPP::Weak::MD5 hash;

    // Create a string to store the hexadecimal hash
    std::string digest;

    // Create a new Hash Filter that feeds into a Hex Encoder,
    // and then add the Hex Encoder's output to the digest string
    CryptoPP::StringSource(input, true, 
        new CryptoPP::HashFilter(hash, 
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(digest)
            )
        )
    );

    std::cerr << "Hash: " << digest << std::endl;
    // Return the hex encoded hash value
    return digest;
}



//wait for ACKSYMBOL and start a thread that waits for 60 seconds and then sends then sends the packet again