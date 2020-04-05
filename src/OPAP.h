#pragma once

#include "LSB.h"

class OPAP: public LSB
{
public:
    OPAP(int usedBits = 1);
    virtual ~OPAP();
    virtual double hide(std::string secretMessage, CImg<unsigned char>& coverImage);
    int getAdjustment(int valueBefore, int valueAfter);
private:


};

