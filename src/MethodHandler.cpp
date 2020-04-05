#include "MethodHandler.h"

MethodHandler::MethodHandler() {

}

MethodHandler::~MethodHandler() {
}

int MethodHandler::getMethodType(const CImg<unsigned char>& stegoImage)
{
    if(1 == BitsOperations::getBitAt(stegoImage(1,0), 1))
        if(1 == BitsOperations::getBitAt(stegoImage(0,0), 1))
            return TYPE_OPAP;
        else
            return TYPE_XOR_LSB;
    else if(1 == BitsOperations::getBitAt(stegoImage(0,0), 1))
        return TYPE_INVERTED_LSB;
    else
        return TYPE_BASIC_LSB;
}

void MethodHandler::setMethodType(CImg<unsigned char>& stegoImage, int type)
{
    switch(type)
    {
        case TYPE_BASIC_LSB:
            BitsOperations::setBitAt(stegoImage(0,0), 1, 0);
            BitsOperations::setBitAt(stegoImage(1,0), 1, 0);
            break;
        case TYPE_INVERTED_LSB:
            BitsOperations::setBitAt(stegoImage(0,0), 1, 1);
            BitsOperations::setBitAt(stegoImage(1,0), 1, 0);
            break;
        case TYPE_XOR_LSB:
            BitsOperations::setBitAt(stegoImage(0,0), 1, 0);
            BitsOperations::setBitAt(stegoImage(1,0), 1, 1);
            break;

        case TYPE_OPAP:
            BitsOperations::setBitAt(stegoImage(0,0), 1, 1);
            BitsOperations::setBitAt(stegoImage(1,0), 1, 1);
            break;
        default:
            break;
    }
}

int MethodHandler::getUsedBitsCode(const CImg<unsigned char>& stegoImage)
{
    //Big Endian
    auto code = (stegoImage(2,0) & BitsOperations::getMaskForBits(1)) << 1
              | (stegoImage(3,0) & BitsOperations::getMaskForBits(1));

    return code;
}

void MethodHandler::setUsedBitsCode(CImg<unsigned char>& stegoImage, int usedBitsCode)
{
    auto msbBit = BitsOperations::getBitAt(usedBitsCode, 1);
    auto lsbBit = BitsOperations::getBitAt(usedBitsCode, 2);

    BitsOperations::setBitAt(stegoImage(2,0), 1, lsbBit);
    BitsOperations::setBitAt(stegoImage(3,0), 1, msbBit);
}

int MethodHandler::getUsedBits(const CImg<unsigned char>& stegoImage)
{
    auto usedBitsCode = getUsedBitsCode(stegoImage);
    switch(usedBitsCode)
    {
        case USED_BITS_1: return 1;
        case USED_BITS_2: return 2;
        case USED_BITS_4: return 4;
        case USED_BITS_8: return 8;
        default:          return 1;
    }
}

void MethodHandler::setUsedBits(CImg<unsigned char>& stegoImage, int usedBits)
{
    switch(usedBits)
    {
        case 1:  setUsedBitsCode(stegoImage, USED_BITS_1); break;
        case 2:  setUsedBitsCode(stegoImage, USED_BITS_2); break;
        case 4:  setUsedBitsCode(stegoImage, USED_BITS_4); break;
        case 8:  setUsedBitsCode(stegoImage, USED_BITS_8); break;
        default: setUsedBitsCode(stegoImage, USED_BITS_1); break;
    }
}
