#pragma once

#include "Common.h"

class MethodHandler {
public:
    MethodHandler();
    virtual ~MethodHandler();

    static int getMethodType(const CImg<unsigned char>& stegoImage);
    static void setMethodType(CImg<unsigned char>& stegoImage, int type);

    static int getUsedBits(const CImg<unsigned char>& stegoImage);
    static void setUsedBits(CImg<unsigned char>& stegoImage, int usedBits);

    static int getUsedBitsCode(const CImg<unsigned char>& stegoImage);
    static void setUsedBitsCode(CImg<unsigned char>& stegoImage, int usedBits);

    //This enum represents following bit orders:
    // ___ ___ ___ ___ ___ ___ ___ ___ ___
    //  n   m   rest of the image pixels
    // (which stand for:)
    //  0   0   Basic LSB
    //  0   1   Inverted LSB
    //  1   0   XOR LSB
    //  1   1   OPAP

    enum
    {
        TYPE_BASIC_LSB = 0,
        TYPE_XOR_LSB = 1,
        TYPE_INVERTED_LSB = 2,
        TYPE_OPAP = 3
    };

    enum
    {
        USED_BITS_1 = 0,
        USED_BITS_2 = 1,
        USED_BITS_4 = 2,
        USED_BITS_8 = 3
    };
};

