#include <bitset>
#include <ctime>

#include "ImageContext.h"
#include "LSB.h"
#include "LSBXor.h"
#include "LSBInverted.h"
#include "OPAP.h"
#include "MethodHandler.h"

ImageContext::ImageContext(CImg<unsigned char>& anImage) :
    originalImage(anImage), basicLsb(anImage), xorLsb(anImage), invertedLsb(anImage), opap(anImage)
{     
};


ImageContext::ImageContext()
{
}

ImageContext::~ImageContext()
{ 
};

CImg<unsigned char> ImageContext::getImageLSB(CImg<unsigned char>& anImage, unsigned char channel, unsigned char bits)
{
    auto lsb(CImg<unsigned char>(anImage.width(), anImage.height(), 1, 1));

    auto channelsInImg = anImage.spectrum();

    if(channelsInImg <= channel)
    {
        std::cout << "Image to be processed has not enough channels!" << std::endl;
        return CImg<unsigned char>::identity_matrix(anImage.width()+1);
    }

    BitsOperations bitsOperator;

    cimg_forXY(lsb,x,y)
    {
        lsb(x, y, 0, channel) = anImage(x, y, 0, channel) & bitsOperator.getMaskForBits(bits);
    }
    
    return lsb;
}

int ImageContext::getNumberOfDifferentPixels(CImg<unsigned char>& resultImage)
{
    auto numberOfDiffs = 0;
    cimg_forXY(originalImage, x, y)
    {
        if(originalImage(x,y) - resultImage(x,y) != 0)
            numberOfDiffs++;
    }

    return numberOfDiffs;
}


double ImageContext::getPSNR(CImg<unsigned char>& source, CImg<unsigned char>& result)
{
    auto MSE = getMSE(source, result);
    auto height = source.height();
    auto width = source.width();

    if(result.height() != height || result.width() != width)
    {
        std::cout << "Images sizes mismatch! Exiting\n";
        return -1;
    }

    auto maxI = 255.0; //maximum value of the pixel

    return 10* log10(maxI * maxI  / MSE);
}


double ImageContext::getMSE(CImg<unsigned char>& source, CImg<unsigned char>& result)
{
    auto MSE = 0.0;
    auto height = source.height();
    auto width = source.width();

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            float diff = (source(i,j) - result(i,j));
            MSE += diff*diff;
        }
    }

    return MSE/(height * width);
}

double ImageContext::hideUsingBasicLsb(std::string secretMessage, unsigned char usedBits)
{
    if(1 != usedBits && 2 != usedBits && 4!= usedBits && 8 != usedBits)
    {
        std::cout << "Error! Incorrect number of used bits!\n";
        return APP_FAIL;
    }

    LSB basicLsbPolicy(usedBits);
    MethodHandler::setMethodType(basicLsb, MethodHandler::TYPE_BASIC_LSB);
    MethodHandler::setUsedBits(basicLsb, usedBits);

    return basicLsbPolicy.hide(secretMessage, basicLsb);
}

std::string ImageContext::decodeBasicLsb(const CImg<unsigned char>& stegoImage, unsigned char bits)
{
    LSB basicLsbPolicy(bits);

    return basicLsbPolicy.decode(stegoImage);
}

CImg<unsigned char> ImageContext::getBasicLsb()
{
    return basicLsb;
}


double ImageContext::hideUsingLsbXor(std::string secretMessage)
{
    LSBXor xorLsbPolicy;
    MethodHandler::setMethodType(xorLsb, MethodHandler::TYPE_XOR_LSB);

    return xorLsbPolicy.hide(secretMessage, xorLsb);
}

std::string ImageContext::decodeXorLsb(const CImg<unsigned char>& stegoImage)
{
    LSBXor xorLsbPolicy;


    return xorLsbPolicy.decode(stegoImage);
}

std::string ImageContext::decode(const CImg<unsigned char>& anImage)
{
    auto method = MethodHandler::getMethodType(anImage);
    switch(method)
    {
        case MethodHandler::TYPE_BASIC_LSB:
            return decodeBasicLsb(anImage, MethodHandler::getUsedBits(anImage));
            break;
        case MethodHandler::TYPE_INVERTED_LSB:
            return decodeInvertedLsb(anImage);
            break;
        case MethodHandler::TYPE_XOR_LSB:
            return decodeXorLsb(anImage);
            break;
        case MethodHandler::TYPE_OPAP:
            return decodeOpap(anImage, MethodHandler::getUsedBits(anImage));
            break;
        default:
            return "Incorrect method code!";
    }
}

CImg<unsigned char> ImageContext::getXorLsb()
{
    return xorLsb;
}

CImg<unsigned char> ImageContext::getInvertedLsb()
{
    return invertedLsb;
}

double ImageContext::hideUsingLsbInverted(std::string secretMessage)
{
    LSBInverted invertedLsbPolicy;
    MethodHandler::setMethodType(invertedLsb, MethodHandler::TYPE_INVERTED_LSB);

    return invertedLsbPolicy.hide(secretMessage, invertedLsb);
}

std::string ImageContext::decodeInvertedLsb(const CImg<unsigned char>& anImage)
{
    LSBInverted invertedBitPolicy;

    return invertedBitPolicy.decode(anImage);
}

double ImageContext::hideUsingOpap(std::string secret, unsigned char usedBits)
{
    OPAP opapPolicy(usedBits);
    MethodHandler::setMethodType(opap, MethodHandler::TYPE_OPAP);
    MethodHandler::setUsedBits(opap, usedBits);

    return opapPolicy.hide(secret, opap);
}

std::string ImageContext::decodeOpap(const CImg<unsigned char>& anImage, unsigned char usedBits)
{
    OPAP opapPolicy(usedBits);

    return opapPolicy.decode(anImage);
}

CImg<unsigned char> ImageContext::getOpap()
{
    return opap;
}

