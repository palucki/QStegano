#pragma once

#include "Hide.h"
#include "Decode.h"
#include "Common.h"

class LSBXor : public Hide, public Decode, public BitsOperations
{
public:
    virtual ~LSBXor();

    virtual int getNumberOfPixelsNeededForOneChar();
    virtual int getNumberOfCarrierBitsFromOneChar();

    unsigned char getX1(unsigned char pixel);
    unsigned char getX2(unsigned char pixel);

    virtual char decodeChar(CImg<unsigned char>::const_iterator& imgIt);
    virtual void hideChar(char secretCharacter);
private:
};

