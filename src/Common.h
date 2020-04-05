#pragma once

#include <iostream>
#include <sstream>
#include <memory>
#include <typeinfo>

// Each stego image will be ordered as follows:
// CONTROL_BITS (can be modified)   20 pixels
//      METHOD_USED                 2 pixels
//      USED_BITS                   2 pixels
//      CRC32                       16 pixels (2 bits per pixel)
// GROUP_INFO_BITS                  4 pixels
// ...
// SECRET.                          N pixels
// ...
// TRAILING_TAB                     8 pixels

#define CONTROL_BITS 20
#define CHAR_SIZE 8
#define GROUP_INFO_BITS 4
#define ENDING_TAB_SIZE 8

extern const std::string CRC_FAILED_MSG;

#define cimg_OS 2

#include "CImg.h"

using namespace cimg_library;

#define APP_SUCCESS 0
#define APP_FAIL -1

class BitsOperations
{
public:
    static unsigned char getMaskForBits(unsigned char numberOfBits);
    static unsigned char getBitAt(unsigned char pixel, int index);
    static void togglePixelLsb(unsigned char& pixel);
    static void setBitAt(unsigned char& pixel, int index, int bitValue);
};

class CRC
{
public:
    static unsigned long getCrcFromHeader(const CImg<unsigned char>& anImage);
    static void setCrcInHeader(CImg<unsigned char>& anImage, unsigned long crc);
    static unsigned long computeCrcForMessage(std::string secret);
    static bool crcMatches(unsigned long headerCrc, unsigned long computedCrc);
private:
    static const int crcOffset = 4;
    static const int numberOfPixelsForCrc = 16; //2 bits in every pixel
};
