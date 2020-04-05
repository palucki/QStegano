#pragma once

#include "LSB.h"
#include "Common.h"

class LSBInverted : public LSB
{
public:
    LSBInverted();
    virtual ~LSBInverted();

    virtual double hide(std::string secretMessage, CImg<unsigned char>& coverImage);

    int getNumberOfPixelsWith23LsbEqual(int group, bool getChanged = false);
    int getNumberOfChangedPixelsWith23LsbEqual(int group);

    void count23LsbGroupsAndChangedPixelsInThem(CImg<unsigned char>& originalImage, CImg<unsigned char>& coverImage);

    bool shouldTheLsbOfStegoImageBeToggledWhileHiding(int group);

    void embedInfoAboutGroups(CImg<unsigned char>& coverImage);

    void toggleTheLsbOfStegoImageIfNeeded(CImg<unsigned char>& coverImage);

    virtual std::string decode(const CImg<unsigned char>& stegoImage);

    virtual char decodeChar(CImg<unsigned char>::const_iterator& imgIt) override;

    int getGroupInfo(const CImg<unsigned char>& coverImage, int group);

private:
    unsigned char get23LsbValue(unsigned char pixel);

    static const int BITS_USED_FOR_INVERTED_LSB = 1;
    static const int NUMBER_OF_GROUPS = 4;

    int numberOfPixelsWith00As23Lsb;
    int numberOfChangedPixelsWith00As23Lsb;

    int numberOfPixelsWith01As23Lsb;
    int numberOfChangedPixelsWith01As23Lsb;

    int numberOfPixelsWith10As23Lsb;
    int numberOfChangedPixelsWith10As23Lsb;

    int numberOfPixelsWith11As23Lsb;
    int numberOfChangedPixelsWith11As23Lsb;

    bool wasGroupToggled[NUMBER_OF_GROUPS];
};
