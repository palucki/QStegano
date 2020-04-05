#pragma once
#include "Common.h"

class ImageContext
{
public:
    ImageContext(CImg<unsigned char>& aImage);
    ImageContext();
    ~ImageContext();
//Common functions
    double getPSNR(CImg<unsigned char>& source, CImg<unsigned char>& result);
    double getMSE(CImg<unsigned char>& source, CImg<unsigned char>& result);
    double getMAE();
    CImg<unsigned char> getImageLSB(CImg<unsigned char>& anImage, unsigned char channel, unsigned char bits);
    int getNumberOfDifferentPixels(CImg<unsigned char>& resultImage);
    std::string decode(const CImg<unsigned char>& anImage);

//basic LSB specific methods
    double hideUsingBasicLsb(std::string secret, unsigned char usedBits = 1);
    std::string decodeBasicLsb(const CImg<unsigned char>& anImage, unsigned char bits);
    CImg<unsigned char> getBasicLsb();

//LSB Xor specific methods
    double hideUsingLsbXor(std::string secret);
    std::string decodeXorLsb(const CImg<unsigned char>& anImage);
    CImg<unsigned char> getXorLsb();

//LSB inverted specific methods
    double hideUsingLsbInverted(std::string secret);
    std::string decodeInvertedLsb(const CImg<unsigned char>& anImage);
    CImg<unsigned char> getInvertedLsb();

//LSB inverted specific methods
    double hideUsingOpap(std::string secret, unsigned char usedBits = 1);
    std::string decodeOpap(const CImg<unsigned char>& anImage, unsigned char usedBits);
    CImg<unsigned char> getOpap();

private:
    CImg<unsigned char> originalImage;
    CImg<unsigned char> basicLsb;
    CImg<unsigned char> xorLsb;
    CImg<unsigned char> invertedLsb;
    CImg<unsigned char> opap;
};
