#pragma once
#include "Common.h"

//template method design pattern
class Hide
{
public:
    virtual ~Hide();
    virtual int getNumberOfCarrierBitsFromOneChar() = 0;
    virtual void hideChar(char secretCharacter) = 0;
    double hide(std::string secretMessage, CImg<unsigned char>& coverImage, int startFrom = 0);
    std::string prepareSecret(const std::string& inputString);
protected:
    CImg<unsigned char>::iterator hidingIterator;
};
