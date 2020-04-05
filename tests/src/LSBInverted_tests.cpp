#include "UnitTestsCommon.h"
#include "LSBInverted.h"

class LSBInverted_Suite : public ::testing::Test
{
protected:
    LSBInverted_Suite()
    {
    }
    virtual ~LSBInverted_Suite()
    {
    }
    LSBInverted lsbInvertedPolicy;
};

TEST_F(LSBInverted_Suite, getNumbersOf2nd3rdCommonGroupsFromImage)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0x00);
    anImage(CONTROL_BITS + 4,0) = 0x00 ; anImage(CONTROL_BITS + 5,0) = 0x01; anImage(CONTROL_BITS + 6,0) = 0x02; anImage(CONTROL_BITS + 7,0) = 0x03;

    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, anImage);

    auto numberOf00As2nd3rd = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(0);
    auto numberOf01As2nd3rd = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(1);
    auto numberOf10As2nd3rd = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(2);
    auto numberOf11As2nd3rd = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(3);

    EXPECT_EQ(2, numberOf00As2nd3rd);
    EXPECT_EQ(2, numberOf01As2nd3rd);
    EXPECT_EQ(0, numberOf11As2nd3rd);
    EXPECT_EQ(0, numberOf10As2nd3rd);
}


TEST_F(LSBInverted_Suite, ForTheSameImageNoPixelsAreChanged)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0x00);
    anImage(CONTROL_BITS + 4,0) = 0x00 ; anImage(CONTROL_BITS + 5,0) = 0x01; anImage(CONTROL_BITS + 6,0) = 0x02; anImage(CONTROL_BITS + 7,0) = 0x03;

    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, anImage);
    auto numberOfChangedWith00As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(0);
    auto numberOfChangedWith01As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(1);
    auto numberOfChangedWith10As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(2);
    auto numberOfChangedWith11As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(3);

    EXPECT_EQ(0, numberOfChangedWith00As2nd3rd);
    EXPECT_EQ(0, numberOfChangedWith01As2nd3rd);
    EXPECT_EQ(0, numberOfChangedWith11As2nd3rd);
    EXPECT_EQ(0, numberOfChangedWith10As2nd3rd);
}

TEST_F(LSBInverted_Suite, ForDifferentImagesThatCorrectNumbersOfChangedPixelsAreReported)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    auto differentImage = anImage;

    //fill with the same group '11' at 23 lsb
    anImage.fill(0xFF);
    differentImage.fill(0xFF);

    //change one pixel at group '11'
    differentImage(CONTROL_BITS + 5,0) = 0xFE;

    //modify the same pixel in both images to be in the group '10' and change LSB of one of them
    anImage(CONTROL_BITS + 4,0) = 0x04;
    differentImage(CONTROL_BITS + 4,0) = 0x05;

    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, differentImage);

    auto numberOfChangedWith00As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(0);
    auto numberOfChangedWith01As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(1);
    auto numberOfChangedWith10As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(2);
    auto numberOfChangedWith11As2nd3rd = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(3);

    EXPECT_EQ(0, numberOfChangedWith00As2nd3rd);
    EXPECT_EQ(0, numberOfChangedWith01As2nd3rd);
    EXPECT_EQ(1, numberOfChangedWith10As2nd3rd);
    EXPECT_EQ(1, numberOfChangedWith11As2nd3rd);
}

TEST_F(LSBInverted_Suite, WhenAllPixelsInTheSameGroupAndNumberOfChangedPixelsInLsbGreaterThanNumberOfUnchangedRevertTheChangeInLsb)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0xFF);

    auto stegoImage = anImage;
    stegoImage.fill(0xFE);

    auto group = 3; //0xFF and 0xFE has 3 ('11') as 23 LSB
    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, stegoImage);

    auto totalNumberOfPixelsInGroup = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(group);
    auto numberOfChangedPixelsInGroup = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(group);
    auto numberOfUnchangedPixelsInGroup = totalNumberOfPixelsInGroup - numberOfChangedPixelsInGroup;

    EXPECT_EQ(4, totalNumberOfPixelsInGroup);
    EXPECT_EQ(4, numberOfChangedPixelsInGroup);
    EXPECT_EQ(0, numberOfUnchangedPixelsInGroup);
    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(group));
}

TEST_F(LSBInverted_Suite, WhenAllPixelsInTheSameGroupAndNumberOfChangedPixelsInLsbSmallerThanNumberOfUnchangedDoNotRevertTheChangeInLsb)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0xFF);

    auto stegoImage = anImage;
    stegoImage(CONTROL_BITS + 4,0) = 0xFE;

    auto group = 3; //0xFF and 0xFE has 3 ('11') as 23 LSB
    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, stegoImage);

    auto totalNumberOfPixelsInGroup = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(group);
    auto numberOfChangedPixelsInGroup = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(group);
    auto numberOfUnchangedPixelsInGroup = totalNumberOfPixelsInGroup - numberOfChangedPixelsInGroup;

    EXPECT_EQ(4, totalNumberOfPixelsInGroup);
    EXPECT_EQ(1, numberOfChangedPixelsInGroup);
    EXPECT_EQ(3, numberOfUnchangedPixelsInGroup);
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(group));
}

TEST_F(LSBInverted_Suite, WhenAllPixelsInTheSameGroupAndNumberOfChangedPixelsInLsbEqualToNumberOfUnchangedDoNotRevertTheChangeInLsb)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0xFF);

    auto stegoImage = anImage;
    stegoImage(CONTROL_BITS + 4,0) = 0xFE;
    stegoImage(CONTROL_BITS + 5,0) = 0xFE;

    auto group = 3; //0xFF and 0xFE has 3 ('11') as 23 LSB
    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, stegoImage);

    auto totalNumberOfPixelsInGroup = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(group);
    auto numberOfChangedPixelsInGroup = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(group);
    auto numberOfUnchangedPixelsInGroup = totalNumberOfPixelsInGroup - numberOfChangedPixelsInGroup;

    EXPECT_EQ(4, totalNumberOfPixelsInGroup);
    EXPECT_EQ(2, numberOfChangedPixelsInGroup);
    EXPECT_EQ(2, numberOfUnchangedPixelsInGroup);
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(group));
}


TEST_F(LSBInverted_Suite, WhenPixelsNotInTheSameGroupAndNumberOfChangedPixelsInLsbGreaterThanNumberOfUnchangedRevertTheChangeInLsb)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 6,1);
    anImage.fill(0x00);
    auto stegoImage = anImage;

    //group 2
    anImage(CONTROL_BITS + 4,0) = 0x04;
    stegoImage(CONTROL_BITS + 4,0) = 0x05;

    //group 1
    anImage(CONTROL_BITS + 5,0) = 0x02;
    stegoImage(CONTROL_BITS + 5,0) = 0x03;

    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, stegoImage);

    auto totalNumberOfPixelsInGroup1 = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(1);
    auto numberOfChangedPixelsInGroup1 = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(1);
    auto numberOfUnchangedPixelsInGroup1 = totalNumberOfPixelsInGroup1 - numberOfChangedPixelsInGroup1;

    auto totalNumberOfPixelsInGroup2 = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(2);
    auto numberOfChangedPixelsInGroup2 = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(2);
    auto numberOfUnchangedPixelsInGroup2 = totalNumberOfPixelsInGroup2 - numberOfChangedPixelsInGroup2;

    EXPECT_EQ(1, totalNumberOfPixelsInGroup1);
    EXPECT_EQ(1, numberOfChangedPixelsInGroup1);
    EXPECT_EQ(0, numberOfUnchangedPixelsInGroup1);
    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(1));

    EXPECT_EQ(1, totalNumberOfPixelsInGroup2);
    EXPECT_EQ(1, numberOfChangedPixelsInGroup2);
    EXPECT_EQ(0, numberOfUnchangedPixelsInGroup2);
    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(1));
}

TEST_F(LSBInverted_Suite, WhenPixelsNotInTheSameGroupAndNumberOfChangedPixelsInLsbSmallerOrEqualToNumberOfUnchangedDoNotRevertTheChangeInLsb)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 7,1);
    anImage.fill(0x00);
    auto stegoImage = anImage;

    //group 2 - number of changed equal to number of unchanged
    anImage(CONTROL_BITS + 4,0) = 0x04;
    stegoImage(CONTROL_BITS + 4,0) = 0x05;
    anImage(CONTROL_BITS + 6,0) = 0x04;
    stegoImage(CONTROL_BITS + 6,0) = 0x04;

    //group 1 - number of changed less than number of unchanged
    anImage(CONTROL_BITS + 5,0) = 0x02;
    stegoImage(CONTROL_BITS + 5,0) = 0x02;

    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, stegoImage);

    auto totalNumberOfPixelsInGroup1 = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(1);
    auto numberOfChangedPixelsInGroup1 = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(1);
    auto numberOfUnchangedPixelsInGroup1 = totalNumberOfPixelsInGroup1 - numberOfChangedPixelsInGroup1;

    auto totalNumberOfPixelsInGroup2 = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(2);
    auto numberOfChangedPixelsInGroup2 = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(2);
    auto numberOfUnchangedPixelsInGroup2 = totalNumberOfPixelsInGroup2 - numberOfChangedPixelsInGroup2;

    EXPECT_EQ(1, totalNumberOfPixelsInGroup1);
    EXPECT_EQ(0, numberOfChangedPixelsInGroup1);
    EXPECT_EQ(1, numberOfUnchangedPixelsInGroup1);
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(1));

    EXPECT_EQ(2, totalNumberOfPixelsInGroup2);
    EXPECT_EQ(1, numberOfChangedPixelsInGroup2);
    EXPECT_EQ(1, numberOfUnchangedPixelsInGroup2);
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(1));
}

TEST_F(LSBInverted_Suite, WhenLsbForGroupShouldBeRevertedEmbedOnesInFirst4BitsOfStegoImage)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0);
    auto stegoImage(anImage);

    stegoImage(CONTROL_BITS + 4,0) = 0x01; stegoImage(CONTROL_BITS + 5,0) = 0x03; stegoImage(CONTROL_BITS + 6,0) = 0x05; stegoImage(CONTROL_BITS + 7,0) = 0x07;

    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, stegoImage);
    lsbInvertedPolicy.embedInfoAboutGroups(stegoImage);

    auto stegoImageBitForGroup0Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 0,0), 1);
    auto stegoImageBitForGroup1Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 1,0), 1);
    auto stegoImageBitForGroup2Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 2,0), 1);
    auto stegoImageBitForGroup3Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 3,0), 1);

    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(0));
    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(1));
    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(2));
    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(3));
    EXPECT_EQ(1, stegoImageBitForGroup0Toggling);
    EXPECT_EQ(1, stegoImageBitForGroup1Toggling);
    EXPECT_EQ(1, stegoImageBitForGroup2Toggling);
    EXPECT_EQ(1, stegoImageBitForGroup3Toggling);
}

TEST_F(LSBInverted_Suite, WhenLsbForGroupShouldNotBeRevertedEmbedZeroesInFirst4BitsOfStegoImage)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0xFF);
    auto stegoImage(anImage);

    lsbInvertedPolicy.count23LsbGroupsAndChangedPixelsInThem(anImage, stegoImage);
    lsbInvertedPolicy.embedInfoAboutGroups(stegoImage);

    auto stegoImageBitForGroup0Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 0,0), 1);
    auto stegoImageBitForGroup1Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 1,0), 1);
    auto stegoImageBitForGroup2Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 2,0), 1);
    auto stegoImageBitForGroup3Toggling = BitsOperations::getBitAt(stegoImage(CONTROL_BITS + 3,0), 1);

    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(0));
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(1));
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(2));
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(3));
    EXPECT_EQ(0, stegoImageBitForGroup0Toggling);
    EXPECT_EQ(0, stegoImageBitForGroup1Toggling);
    EXPECT_EQ(0, stegoImageBitForGroup2Toggling);
    EXPECT_EQ(0, stegoImageBitForGroup3Toggling);
}

TEST_F(LSBInverted_Suite, HideCharWithoutTogglingLsbSingleGroup)
{
    auto numberOfCarrierPixels = 16;
    auto numberOfGroupInfoPixels = GROUP_INFO_BITS;
    auto numberOfControlBits = CONTROL_BITS;
    auto imageWidth = numberOfCarrierPixels + numberOfGroupInfoPixels + numberOfControlBits;
    CImg<unsigned char> anImage(imageWidth, 1); //4 for groups info, 8 for 'a' and 8 for '\t'
    anImage.fill(0);
    auto group = 0;

    //'\t' is 0000 1001
    //'\0' is 0000 0000

    auto secretMessage = std::string("\0", 1); //secret way to store null in string :D

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto numberOfPixelsInGroup = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(group);
    auto numberOfChangedPixels = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(group);
    auto togglePixelsForGroup0 = numberOfChangedPixels > (numberOfPixelsInGroup - numberOfChangedPixels) ? 1 : 0;

    EXPECT_EQ(numberOfCarrierPixels, numberOfPixelsInGroup);
    EXPECT_EQ(2, numberOfChangedPixels);
    EXPECT_EQ(false, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(group));

    //group info bits                                                //bits for '\0'                 //bits for '\t'
    EXPECT_EQ(togglePixelsForGroup0, anImage(CONTROL_BITS + 0,0));    EXPECT_EQ(0, anImage(CONTROL_BITS +4,0));     EXPECT_EQ(1, anImage(CONTROL_BITS +12,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS +1,0));                        EXPECT_EQ(0, anImage(CONTROL_BITS +5,0));     EXPECT_EQ(0, anImage(CONTROL_BITS +13,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS +2,0));                        EXPECT_EQ(0, anImage(CONTROL_BITS +6,0));     EXPECT_EQ(0, anImage(CONTROL_BITS +14,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS +3,0));                        EXPECT_EQ(0, anImage(CONTROL_BITS +7,0));     EXPECT_EQ(1, anImage(CONTROL_BITS +15,0));
                                                                     EXPECT_EQ(0, anImage(CONTROL_BITS +8,0));     EXPECT_EQ(0, anImage(CONTROL_BITS +16,0));
                                                                     EXPECT_EQ(0, anImage(CONTROL_BITS +9,0));     EXPECT_EQ(0, anImage(CONTROL_BITS +17,0));
                                                                     EXPECT_EQ(0, anImage(CONTROL_BITS +10,0));    EXPECT_EQ(0, anImage(CONTROL_BITS +18,0));
                                                                     EXPECT_EQ(0, anImage(CONTROL_BITS +11,0));    EXPECT_EQ(0, anImage(CONTROL_BITS +19,0));
}

TEST_F(LSBInverted_Suite, HideCharTogglingLsbSingleGroup)
{
    auto numberOfCarrierPixels = 16;
    auto imageWidth = numberOfCarrierPixels + GROUP_INFO_BITS + CONTROL_BITS;
    CImg<unsigned char> anImage(imageWidth, 1); //4 for groups info, 8 for 'a' and 8 for '\t'
    anImage.fill(0x01);
    auto group = 0;

    //'\t' is 0000 1001
    //'\0' is 0000 0000

    auto secretMessage = std::string("\0", 1); //secret way to store null in string :D

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto numberOfPixelsInGroup = lsbInvertedPolicy.getNumberOfPixelsWith23LsbEqual(group);
    auto numberOfChangedPixels = lsbInvertedPolicy.getNumberOfChangedPixelsWith23LsbEqual(group);
    auto togglePixelsForGroup0 = numberOfChangedPixels > (numberOfPixelsInGroup - numberOfChangedPixels) ? 1 : 0;

    EXPECT_EQ(numberOfCarrierPixels, numberOfPixelsInGroup);
    EXPECT_EQ(14, numberOfChangedPixels);
    EXPECT_EQ(true, lsbInvertedPolicy.shouldTheLsbOfStegoImageBeToggledWhileHiding(group));

    //group info bits                                                  //bits for '\0'                 //bits for '\t'
    EXPECT_EQ(togglePixelsForGroup0, anImage(CONTROL_BITS + 0,0));     EXPECT_EQ(1, anImage(CONTROL_BITS + 4,0));     EXPECT_EQ(0, anImage(CONTROL_BITS + 12,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 1,0));                         EXPECT_EQ(1, anImage(CONTROL_BITS + 5,0));     EXPECT_EQ(1, anImage(CONTROL_BITS + 13,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 2,0));                         EXPECT_EQ(1, anImage(CONTROL_BITS + 6,0));     EXPECT_EQ(1, anImage(CONTROL_BITS + 14,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 3,0));                         EXPECT_EQ(1, anImage(CONTROL_BITS + 7,0));     EXPECT_EQ(0, anImage(CONTROL_BITS + 15,0));
                                                                       EXPECT_EQ(1, anImage(CONTROL_BITS + 8,0));     EXPECT_EQ(1, anImage(CONTROL_BITS + 16,0));
                                                                       EXPECT_EQ(1, anImage(CONTROL_BITS + 9,0));     EXPECT_EQ(1, anImage(CONTROL_BITS + 17,0));
                                                                       EXPECT_EQ(1, anImage(CONTROL_BITS + 10,0));    EXPECT_EQ(1, anImage(CONTROL_BITS + 18,0));
                                                                       EXPECT_EQ(1, anImage(CONTROL_BITS + 11,0));    EXPECT_EQ(1, anImage(CONTROL_BITS + 19,0));
}

TEST_F(LSBInverted_Suite, HideCharWithoutTogglingLsbMultipleGroup)
{
    auto numberOfCarrierPixels = 16;
    auto imageWidth = numberOfCarrierPixels + + GROUP_INFO_BITS + CONTROL_BITS;
    CImg<unsigned char> anImage(imageWidth, 1); //4 for groups info, 8 for 'a' and 8 for '\t'

    //group info                        //group 0                           //group 1                            //group 2                           //group 3
    anImage(CONTROL_BITS + 0,0) = 1;   anImage(CONTROL_BITS + 4,0) = 0;   anImage(CONTROL_BITS + 8,0)  = 2;    anImage(CONTROL_BITS + 12,0) = 4;    anImage(CONTROL_BITS + 16,0) = 6;
    anImage(CONTROL_BITS + 1,0) = 1;   anImage(CONTROL_BITS + 5,0) = 0;   anImage(CONTROL_BITS + 9,0)  = 2;    anImage(CONTROL_BITS + 13,0) = 4;    anImage(CONTROL_BITS + 17,0) = 6;
    anImage(CONTROL_BITS + 2,0) = 1;   anImage(CONTROL_BITS + 6,0) = 0;   anImage(CONTROL_BITS + 10,0) = 2;    anImage(CONTROL_BITS + 14,0) = 4;    anImage(CONTROL_BITS + 18,0) = 6;
    anImage(CONTROL_BITS + 3,0) = 1;   anImage(CONTROL_BITS + 7,0) = 0;   anImage(CONTROL_BITS + 11,0) = 2;    anImage(CONTROL_BITS + 15,0) = 4;    anImage(CONTROL_BITS + 19,0) = 6;

    //'\t' is 0000 1001
    //'\0' is 0000 0000

    auto secretMessage = std::string("\0", 1); //secret way to store null in string :D

    lsbInvertedPolicy.hide(secretMessage, anImage);

    int togglePixelsForGroup[4] = {0, 0, 0, 0};

    EXPECT_EQ(togglePixelsForGroup[0], anImage(CONTROL_BITS + 0,0));
    EXPECT_EQ(togglePixelsForGroup[1], anImage(CONTROL_BITS + 1,0));
    EXPECT_EQ(togglePixelsForGroup[2], anImage(CONTROL_BITS + 2,0));
    EXPECT_EQ(togglePixelsForGroup[3], anImage(CONTROL_BITS + 3,0));

    //bits for '\0'                                 //bits for '\t'
    EXPECT_EQ(0, anImage(CONTROL_BITS + 4,0));     EXPECT_EQ(5, anImage(CONTROL_BITS + 12,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 5,0));     EXPECT_EQ(4, anImage(CONTROL_BITS + 13,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 6,0));     EXPECT_EQ(4, anImage(CONTROL_BITS + 14,0));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 7,0));     EXPECT_EQ(5, anImage(CONTROL_BITS + 15,0));
    EXPECT_EQ(2, anImage(CONTROL_BITS + 8,0));     EXPECT_EQ(6, anImage(CONTROL_BITS + 16,0));
    EXPECT_EQ(2, anImage(CONTROL_BITS + 9,0));     EXPECT_EQ(6, anImage(CONTROL_BITS + 17,0));
    EXPECT_EQ(2, anImage(CONTROL_BITS + 10,0));    EXPECT_EQ(6, anImage(CONTROL_BITS + 18,0));
    EXPECT_EQ(2, anImage(CONTROL_BITS + 11,0));    EXPECT_EQ(6, anImage(CONTROL_BITS + 19,0));
}

TEST_F(LSBInverted_Suite, HideCharToggling3GroupsAndDoNotToggleTheOneThatHasEqualNumberOfChangedAndUnchanged)
{
    auto numberOfCarrierPixels = 16;
    auto numberOfGroupInfoPixels = 4;
    auto imageWidth = numberOfCarrierPixels + numberOfGroupInfoPixels + CONTROL_BITS;
    CImg<unsigned char> anImage(imageWidth, 1); //4 for groups info, 8 for 'a' and 8 for '\t'

    //group info        //group 0           //group 1             //group 2             //group 3
    anImage(CONTROL_BITS +  0,0) = 0;   anImage(CONTROL_BITS +  4,0) = 1;   anImage(CONTROL_BITS +  8,0)  = 3;    anImage(CONTROL_BITS + 12,0) = 5;    anImage(CONTROL_BITS + 16,0) = 7;
    anImage(CONTROL_BITS +  1,0) = 0;   anImage(CONTROL_BITS +  5,0) = 1;   anImage(CONTROL_BITS +  9,0)  = 3;    anImage(CONTROL_BITS + 13,0) = 5;    anImage(CONTROL_BITS + 17,0) = 7;
    anImage(CONTROL_BITS +  2,0) = 0;   anImage(CONTROL_BITS +  6,0) = 1;   anImage(CONTROL_BITS +  10,0) = 3;    anImage(CONTROL_BITS + 14,0) = 5;    anImage(CONTROL_BITS + 18,0) = 7;
    anImage(CONTROL_BITS +  3,0) = 0;   anImage(CONTROL_BITS +  7,0) = 1;   anImage(CONTROL_BITS +  11,0) = 3;    anImage(CONTROL_BITS + 15,0) = 5;    anImage(CONTROL_BITS + 19,0) = 7;

    //'\t' is 0000 1001
    //'\0' is 0000 0000

    auto secretMessage = std::string("\0", 1); //secret way to store null in string :D

    lsbInvertedPolicy.hide(secretMessage, anImage);

    int togglePixelsForGroup[4] = {1, 1, 0, 1};

    EXPECT_EQ(togglePixelsForGroup[0], anImage(CONTROL_BITS + 0,0));
    EXPECT_EQ(togglePixelsForGroup[1], anImage(CONTROL_BITS + 1,0));
    EXPECT_EQ(togglePixelsForGroup[2], anImage(CONTROL_BITS + 2,0));
    EXPECT_EQ(togglePixelsForGroup[3], anImage(CONTROL_BITS + 3,0));

    //bits for '\0'                                 //bits for '\t'
    EXPECT_EQ(1, anImage(CONTROL_BITS + 4,0));     EXPECT_EQ(5, anImage(CONTROL_BITS + 12,0));
    EXPECT_EQ(1, anImage(CONTROL_BITS + 5,0));     EXPECT_EQ(4, anImage(CONTROL_BITS + 13,0));
    EXPECT_EQ(1, anImage(CONTROL_BITS + 6,0));     EXPECT_EQ(4, anImage(CONTROL_BITS + 14,0));
    EXPECT_EQ(1, anImage(CONTROL_BITS + 7,0));     EXPECT_EQ(5, anImage(CONTROL_BITS + 15,0));
    EXPECT_EQ(3, anImage(CONTROL_BITS + 8,0));     EXPECT_EQ(7, anImage(CONTROL_BITS + 16,0));
    EXPECT_EQ(3, anImage(CONTROL_BITS + 9,0));     EXPECT_EQ(7, anImage(CONTROL_BITS + 17,0));
    EXPECT_EQ(3, anImage(CONTROL_BITS + 10,0));    EXPECT_EQ(7, anImage(CONTROL_BITS + 18,0));
    EXPECT_EQ(3, anImage(CONTROL_BITS + 11,0));    EXPECT_EQ(7, anImage(CONTROL_BITS + 19,0));
}

TEST_F(LSBInverted_Suite, decodeEmptyStringWhenLsbWasNotToggled)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 12, 1);
    anImage.fill(0);
    std::string secretMessage = "";

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto decodedMessage = lsbInvertedPolicy.decode(anImage);

    EXPECT_EQ("", decodedMessage);
}

TEST_F(LSBInverted_Suite, ignoreTabInSecretMessage)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 12, 1);
    anImage.fill(0);
    std::string secretMessage = "\t";

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto decodedMessage = lsbInvertedPolicy.decode(anImage);

    EXPECT_EQ("", decodedMessage);
}

TEST_F(LSBInverted_Suite, getInfoAboutTogglingTheLsbFromImage)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 12, 1);
    anImage.fill(1);
    anImage(CONTROL_BITS + 0,0) = 0; //to check if is set correctly

    auto secretMessage = "\t";

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto group0Toggled = lsbInvertedPolicy.getGroupInfo(anImage, 0);
    auto group1Toggled = lsbInvertedPolicy.getGroupInfo(anImage, 1);
    auto group2Toggled = lsbInvertedPolicy.getGroupInfo(anImage, 2);
    auto group3Toggled = lsbInvertedPolicy.getGroupInfo(anImage, 3);

    EXPECT_EQ(1 ,group0Toggled);
    EXPECT_EQ(0 ,group1Toggled);
    EXPECT_EQ(0 ,group2Toggled);
    EXPECT_EQ(0 ,group3Toggled);
}

TEST_F(LSBInverted_Suite, decodeEmptyStringWhenLsbWasToggled)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 12, 1);
    anImage.fill(1);
    anImage(CONTROL_BITS + 0,0) = 0; //to check if is set correctly

    auto secretMessage = "";

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto decodedMessage = lsbInvertedPolicy.decode(anImage);

    auto group0Toggled = lsbInvertedPolicy.getGroupInfo(anImage, 0);

    EXPECT_EQ(1 ,group0Toggled);
    EXPECT_EQ(secretMessage, decodedMessage);
}

TEST_F(LSBInverted_Suite, decodeStringWhenLsbWasNotToggled)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 20, 1);
    anImage.fill(0);
    std::string secretMessage = "r"; //'r' has 4 '1' and 4 '0' so should not be toggled

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto decodedMessage = lsbInvertedPolicy.decode(anImage);
    auto group0Toggled = lsbInvertedPolicy.getGroupInfo(anImage, 0);

    EXPECT_EQ(0 ,group0Toggled);
    EXPECT_EQ(secretMessage, decodedMessage);
}

TEST_F(LSBInverted_Suite, decodeStringWhenLsbWasToggled)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 20, 1);
    anImage.fill(1);

    std::string secretMessage = "r"; //'r' has 4 '1' and 4 '0' so should not be toggled

    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto decodedMessage = lsbInvertedPolicy.decode(anImage);

    auto group0Toggled = lsbInvertedPolicy.getGroupInfo(anImage, 0);

    EXPECT_EQ(1 ,group0Toggled);
    EXPECT_EQ(secretMessage, decodedMessage);
}

TEST_F(LSBInverted_Suite, acceptanceTest)
{
    CImg<unsigned char> anImage(201, 2);
    anImage.fill(0xC3);

    std::string secretMessage = "Steganography rocks!!!111";
    lsbInvertedPolicy.hide(secretMessage, anImage);

    auto decodedMessage = lsbInvertedPolicy.decode(anImage);

    EXPECT_EQ(secretMessage, decodedMessage);
}

TEST_F(LSBInverted_Suite, decodeFromImageWithoutTabAtTheEnd)
{

}




