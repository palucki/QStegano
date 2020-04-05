#include <chrono>
#include "OPAP.h"

OPAP::OPAP(int usedBits) : LSB(usedBits)
{
}

OPAP::~OPAP()
{
}

int OPAP::getAdjustment(int valueBefore, int valueAfter)
{
    auto pow2p_1 = pow(2, usedBits-1);
    auto pow2p = pow(2, usedBits);
    auto decimalLsbBefore = valueBefore & getMaskForBits(usedBits); //vp
    auto decimalLsbAfter = valueAfter & getMaskForBits(usedBits);   //q
    auto lsbDiff = int(decimalLsbBefore-decimalLsbAfter);

    if(lsbDiff > pow2p_1 && valueAfter + pow2p <= 255)
    {
        return pow2p;
    }
    else if(lsbDiff < -pow2p_1 && valueAfter - pow2p >= 0)
    {
        return -pow2p;
    }
    else
    {
        return 0;
    }
}

double OPAP::hide(std::string secretMessage, CImg<unsigned char>& coverImage)
{
    auto originalImage = coverImage;
    auto timeSpent = LSB::hide(secretMessage, coverImage);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    cimg_forXY(coverImage, x,y)
    {
        coverImage(x,y) += getAdjustment(originalImage(x,y), coverImage(x,y));
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    timeSpent += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    return timeSpent;
}
