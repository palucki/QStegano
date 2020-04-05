#include "LSB.h"

LSB::LSB(int _usedBits) : usedBits(_usedBits)
{
}
LSB::~LSB() {}
char LSB::decodeChar(CImg<unsigned char>::const_iterator& imgIt)
{
    auto pixelsForOneChar = 8/usedBits;
    char decodedCharacter = 0;

    for(int j = 0; j < pixelsForOneChar; j++)
    {
        unsigned char pixel = (*imgIt++) & getMaskForBits(usedBits);
        decodedCharacter |= (pixel << j*usedBits);
    }

    return decodedCharacter;
}

int LSB::getNumberOfPixelsNeededForOneChar()
{
    return 8/usedBits;
}

int LSB::getNumberOfCarrierBitsFromOneChar()
{
    return usedBits;
}

unsigned char LSB::getBitsFromSecret(unsigned char secret, unsigned char startPos, unsigned char numberOfBits)
{
    secret >>= startPos;
    secret &= getMaskForBits(numberOfBits);

    return secret;
}

void LSB::hideChar(char secretCharacter)
{
    for(auto pos = 0; pos < 8; pos+=usedBits, hidingIterator++)
    {
        (*hidingIterator) &= ~(getMaskForBits(usedBits)); //clear LSB of the secretCharacter
        (*hidingIterator) |= getBitsFromSecret(secretCharacter, pos, usedBits);
    }
}
