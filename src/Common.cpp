#include "Common.h"
#include <QtZlib/zlib.h>

const std::string CRC_FAILED_MSG = "CRC check failed!!!";

unsigned char BitsOperations::getMaskForBits(unsigned char numberOfBits)
{
    return ~(0xFF << numberOfBits);
}

unsigned char BitsOperations::getBitAt(unsigned char pixel, int index)
{
    auto value = pixel & (1 << (index-1));
    if(value)
        return 1;
    else
        return 0;
}

void BitsOperations::togglePixelLsb(unsigned char& pixel)
{
    pixel ^= getMaskForBits(1);
}

void BitsOperations::setBitAt(unsigned char& pixel, int index, int bitValue)
{
    if(0 == bitValue)
    {
        pixel &= ~(1 << (index-1));
    }
    else
    {
        pixel |= (1 << (index-1));
    }
}

bool CRC::crcMatches(unsigned long headerCrc, unsigned long computedCrc)
{
    return headerCrc == computedCrc;
}

unsigned long CRC::getCrcFromHeader(const CImg<unsigned char>& anImage)
{
    unsigned long crc = 0;
    for(unsigned int i = 0; i < numberOfPixelsForCrc; i++)
    {
        unsigned long msb = BitsOperations::getBitAt(anImage(crcOffset + i, 0), 2);
        msb <<= (2*i + 1);
        unsigned long lsb = BitsOperations::getBitAt(anImage(crcOffset + i, 0), 1);
        lsb <<= (2*i);

        crc |= msb;
        crc |= lsb;
    }

    return crc;
}

void CRC::setCrcInHeader(CImg<unsigned char>& anImage, unsigned long crc)
{
    //Little endian in header
    for(int i = 0; i < numberOfPixelsForCrc; i++)
    {
        anImage(crcOffset + i, 0) &= 0xFC; //clear 2 lsb bits;
        anImage(crcOffset + i, 0) |= (crc & (3 << 2*i)) >> 2*i; //set 2 bits from crc
    }
}

unsigned long CRC::computeCrcForMessage(std::string secret)
{
    unsigned long crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (const unsigned char*)secret.c_str(), secret.length());

    return crc;
}
