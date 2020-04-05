#include "LSBXor.h"

LSBXor::~LSBXor() {}

int LSBXor::getNumberOfPixelsNeededForOneChar()
{
    return 8;
}

int LSBXor::getNumberOfCarrierBitsFromOneChar()
{
    return 1;
}

unsigned char LSBXor::getX1(unsigned char pixel)
{
    return getBitAt(pixel, 1) ^ getBitAt(pixel, 8);
}

unsigned char LSBXor::getX2(unsigned char pixel)
{
    return getBitAt(pixel, 2) ^ getBitAt(pixel, 7);
}

char LSBXor::decodeChar(CImg<unsigned char>::const_iterator& imgIt)
{
    auto pixelsForOneChar = 8;
    char decodedCharacter = 0;

    for(int j = 0; j < pixelsForOneChar; j++)
    {
        auto pixel = *imgIt++;
        auto x1 = getX1(pixel);
        auto x2 = getX2(pixel);

        if(x1 == x2)
        {
            decodedCharacter &= ~(1 << j);
        }
        else
        {
            decodedCharacter |= (1 << j);
        }
    }

    return decodedCharacter;
}

void LSBXor::hideChar(char secretCharacter)
{
    for(auto pos = 0; pos < 8 ; pos++, hidingIterator++)
    {
        auto pixel = *hidingIterator;
        auto x1 = getX1(pixel);
        auto x2 = getX2(pixel);
        auto bitToBeHidden = getBitAt(secretCharacter, pos+1);

        if(0 == bitToBeHidden)
        {
            if (x1 != x2)
            {
                togglePixelLsb(pixel);
            }
        }
        else if (1 == bitToBeHidden)
        {
            if (x1 == x2)
            {
                togglePixelLsb(pixel);
            }
        }
        else
        {
            std::cout << "Unexpected bit value (not 0 nor 1)\n";
        }

        *hidingIterator = pixel;
    }
}
