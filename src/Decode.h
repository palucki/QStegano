#pragma once
#include "Common.h"

//template method design pattern
class Decode
{
public:
    virtual ~Decode();
    virtual std::string decode(const CImg<unsigned char>& stegoImage, int startFrom = 0);
    virtual char decodeChar(CImg<unsigned char>::const_iterator& imgIt) = 0;
    virtual int getNumberOfPixelsNeededForOneChar() = 0;
    std::string getMessageFromImage(const CImg<unsigned char>& stegoImage, int startFrom);

    bool foundEndOfSecret(char decodedCharacter);
private:
};
