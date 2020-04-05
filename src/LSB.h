#pragma once

#include "Hide.h"
#include "Decode.h"
#include "Common.h"

class LSB : public Hide, public Decode, public BitsOperations
{
public:
    LSB(int _usedBits);
    virtual ~LSB();
    virtual char decodeChar(CImg<unsigned char>::const_iterator& imgIt);
    virtual int getNumberOfPixelsNeededForOneChar();
    virtual int getNumberOfCarrierBitsFromOneChar();
    unsigned char getBitsFromSecret(unsigned char secret, unsigned char startPos, unsigned char numberOfBits);
//    virtual double complexHideTemplate(std::string secretMessage, CImg<unsigned char>& coverImage, int startFrom = 0);
    virtual void hideChar(char secretCharacter);
protected:
    int usedBits;
};
