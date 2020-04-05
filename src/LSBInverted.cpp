#include <chrono>
#include "LSBInverted.h"

LSBInverted::LSBInverted() : LSB(BITS_USED_FOR_INVERTED_LSB),
                numberOfPixelsWith00As23Lsb(0), numberOfChangedPixelsWith00As23Lsb(0),
                numberOfPixelsWith01As23Lsb(0), numberOfChangedPixelsWith01As23Lsb(0),
                numberOfPixelsWith10As23Lsb(0), numberOfChangedPixelsWith10As23Lsb(0),
                numberOfPixelsWith11As23Lsb(0), numberOfChangedPixelsWith11As23Lsb(0)
{

}
LSBInverted::~LSBInverted() {}

double LSBInverted::hide(std::string secretMessage, CImg<unsigned char>& coverImage)
{
    auto offsetForBitsWithGroupsInfo = GROUP_INFO_BITS;
    auto originalImage = coverImage;
    auto timeSpent = LSB::hide(secretMessage, coverImage, offsetForBitsWithGroupsInfo);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    count23LsbGroupsAndChangedPixelsInThem(originalImage, coverImage);

    embedInfoAboutGroups(coverImage);

    toggleTheLsbOfStegoImageIfNeeded(coverImage);

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    timeSpent += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    return timeSpent;
}

int LSBInverted::getNumberOfPixelsWith23LsbEqual(int group, bool getChanged)
{
    if(group == 0)
    {
        return getChanged ? numberOfChangedPixelsWith00As23Lsb : numberOfPixelsWith00As23Lsb;
    }
    else if (group == 1)
    {
        return getChanged ? numberOfChangedPixelsWith01As23Lsb : numberOfPixelsWith01As23Lsb;
    }
    else if (group == 2)
    {
        return getChanged ? numberOfChangedPixelsWith10As23Lsb : numberOfPixelsWith10As23Lsb;
    }
    else if(group == 3)
    {
        return getChanged ? numberOfChangedPixelsWith11As23Lsb : numberOfPixelsWith11As23Lsb;
    }

    return -1;
}

int LSBInverted::getNumberOfChangedPixelsWith23LsbEqual(int group)
{
    return getNumberOfPixelsWith23LsbEqual(group, true);
}

void LSBInverted::count23LsbGroupsAndChangedPixelsInThem(CImg<unsigned char>& originalImage, CImg<unsigned char>& coverImage)
{
    auto diffImage(coverImage);
    diffImage = abs(originalImage - coverImage);
    auto pixelCounter = 0;

    cimg_forXY(coverImage, x, y)
    {
        auto pixel = coverImage(x,y);
        unsigned char diffAtPixel = diffImage(x,y);

        if(diffAtPixel > 1)
        {
            diffAtPixel = 1;
        }

        if(pixelCounter++ < CONTROL_BITS + GROUP_INFO_BITS)
        {
            //ommit the first 6 pixels for embedding info about groups of 23Lsb
        }
        else if(get23LsbValue(pixel) == 0)
        {
            numberOfPixelsWith00As23Lsb++;
            numberOfChangedPixelsWith00As23Lsb += diffAtPixel;
        }
        else if (get23LsbValue(pixel) == 1)
        {
            numberOfPixelsWith01As23Lsb++;
            numberOfChangedPixelsWith01As23Lsb += diffAtPixel;
        }
        else if (get23LsbValue(pixel) == 2)
        {
            numberOfPixelsWith10As23Lsb++;
            numberOfChangedPixelsWith10As23Lsb += diffAtPixel;
        }
        else if(get23LsbValue(pixel) == 3)
        {
            numberOfPixelsWith11As23Lsb++;
            numberOfChangedPixelsWith11As23Lsb += diffAtPixel;
        }
    }
}

bool LSBInverted::shouldTheLsbOfStegoImageBeToggledWhileHiding(int group)
{
    auto totalNumberOfPixelsInGroup = getNumberOfPixelsWith23LsbEqual(group);
    auto numberOfChangedPixelsInGroup = getNumberOfChangedPixelsWith23LsbEqual(group);
    auto numberOfUnchangedPixelsInGroup = totalNumberOfPixelsInGroup - numberOfChangedPixelsInGroup;

    if(totalNumberOfPixelsInGroup > 0 && numberOfChangedPixelsInGroup >= 0 && numberOfUnchangedPixelsInGroup >= 0)
        return numberOfChangedPixelsInGroup > numberOfUnchangedPixelsInGroup;
    else
        return false;
}


void LSBInverted::embedInfoAboutGroups(CImg<unsigned char>& coverImage)
{
    hidingIterator = coverImage.begin() + CONTROL_BITS;

    for(auto group = 0; group < NUMBER_OF_GROUPS; group++, hidingIterator++)
    {
        if(shouldTheLsbOfStegoImageBeToggledWhileHiding(group))
        {
            *hidingIterator |= 0x01;
        }
        else
        {
            *hidingIterator &= ~(0x01);
        }
    }
}

void LSBInverted::toggleTheLsbOfStegoImageIfNeeded(CImg<unsigned char>& coverImage)
{
    bool toggleGroup[NUMBER_OF_GROUPS];
    for(auto group = 0; group < NUMBER_OF_GROUPS; group++)
    {
        toggleGroup[group] = shouldTheLsbOfStegoImageBeToggledWhileHiding(group);
    }

    auto pixelCounter = 0;

    cimg_forXY(coverImage, x, y)
    {
        auto pixelGroup = get23LsbValue(coverImage(x,y));

        if(pixelCounter++ < CONTROL_BITS + GROUP_INFO_BITS)
        {
            //ommit the first 4 pixels that hold info about groups of 23Lsb
        }
        else if(toggleGroup[pixelGroup])
        {
            togglePixelLsb(coverImage(x,y));
        }
    }
}

std::string LSBInverted::decode(const CImg<unsigned char>& stegoImage)
{
    for(auto i = 0; i < NUMBER_OF_GROUPS; i++)
    {
        wasGroupToggled[i] = getGroupInfo(stegoImage, i);
    }

    return LSB::decode(stegoImage, NUMBER_OF_GROUPS);
}

char LSBInverted::decodeChar(CImg<unsigned char>::const_iterator& imgIt)
{
    auto pixelsForOneChar = 8/usedBits;
    char decodedCharacter = 0;

    for(int j = 0; j < pixelsForOneChar; j++)
    {
        unsigned char pixel = *imgIt++ & getMaskForBits(usedBits);
        auto pixelGroup = get23LsbValue(pixel);

        if(wasGroupToggled[pixelGroup])
        {
            togglePixelLsb(pixel);
        }

        decodedCharacter |= (pixel << j*usedBits);
    }

    return decodedCharacter;
}

int LSBInverted::getGroupInfo(const CImg<unsigned char>& coverImage, int group)
{
    auto imageIt = coverImage.begin() + CONTROL_BITS + group;
    auto pixel = *imageIt;

    return getBitAt(pixel, 1);
}

unsigned char LSBInverted::get23LsbValue(unsigned char pixel)
{
    unsigned char group = (pixel & getMaskForBits(3)) >> 1;

    return group < NUMBER_OF_GROUPS ? group : 0;
}
