#include "Decode.h"
Decode::~Decode() {}

std::string Decode::decode(const CImg<unsigned char>& stegoImage, int startFrom)
{
    return getMessageFromImage(stegoImage, startFrom);
}

std::string Decode::getMessageFromImage(const CImg<unsigned char>& stegoImage, int startFrom)
{
    std::string message = "";
    auto pixelsForChar = getNumberOfPixelsNeededForOneChar();

    startFrom += CONTROL_BITS;

    for(auto imgIt = stegoImage.begin() + startFrom; imgIt + pixelsForChar <= stegoImage.end(); )
    {
        auto decodedCharacter = decodeChar(imgIt);

        if(foundEndOfSecret(decodedCharacter))
            break;
        else
            message += decodedCharacter;
    }

    auto crcFromHeader = CRC::getCrcFromHeader(stegoImage);
    auto crcForMessage = CRC::computeCrcForMessage(message +'\t'); //still some issues

    if(CRC::crcMatches(crcFromHeader, crcForMessage))
        return message;
    else
    {
        std::cout << std::hex << crcFromHeader << std::endl;
        std::cout << std::hex << crcForMessage << std::endl;
        return "CRC check failed!!!";
    }
}

bool Decode::foundEndOfSecret(char decodedCharacter)
{
    return decodedCharacter == '\t';
}
