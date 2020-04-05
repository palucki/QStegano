#include <chrono>
#include <string>

#include "Hide.h"
#include "MethodHandler.h"

Hide::~Hide() {}

double Hide::hide(std::string secretMessage, CImg<unsigned char>& coverImage, int startFrom)
{
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    if(secretMessage.length() == 0)
        secretMessage += '\t';
    else if(secretMessage[secretMessage.length()-1] != '\t')
        secretMessage += '\t';

    //trim secret from the beginning to the first '\t' character
    std::string trimmedSecretMessage = std::string(secretMessage.begin(), secretMessage.begin() + secretMessage.find("\t"));
    trimmedSecretMessage += '\t';

    secretMessage = trimmedSecretMessage;

    startFrom += CONTROL_BITS;

    int requiredBitsForSecret = 8 * secretMessage.length() + startFrom;
    int availableBitsInImage = getNumberOfCarrierBitsFromOneChar() * coverImage.width() * coverImage.height();

    if(requiredBitsForSecret > availableBitsInImage)
    {
        std::cout << "Error! Available space is not enough!\n";
        std::cout << "Need: " << requiredBitsForSecret << " whereas available: " << availableBitsInImage << "\n";
        return APP_FAIL;
    }

    auto crc = CRC::computeCrcForMessage(secretMessage);
    CRC::setCrcInHeader(coverImage, crc);
    hidingIterator = coverImage.begin() + startFrom;

    for(const char& character : secretMessage)
    {
        hideChar(character);
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
}
