#include "UnitTestsCommon.h"
#include "OPAP.h"

class OPAP_Suite : public ::testing::Test
{
protected:
    OPAP_Suite() : opapPolicy1Bit(OPAP(1)),
                   opapPolicy2Bit(OPAP(2)),
                   opapPolicy4Bit(OPAP(4)),
                   opapPolicy8Bit(OPAP(8))
    {
    }
    virtual ~OPAP_Suite()
    {
    }
    OPAP opapPolicy1Bit;
    OPAP opapPolicy2Bit;
    OPAP opapPolicy4Bit;
    OPAP opapPolicy8Bit;
};

TEST_F(OPAP_Suite, getNumberOfUsedBits)
{
    EXPECT_EQ(4, opapPolicy4Bit.getNumberOfCarrierBitsFromOneChar());
}

TEST_F(OPAP_Suite, embedTabAtTheEnd)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 16,1);
    anImage.fill('0');

    opapPolicy1Bit.hide("U", anImage);

    EXPECT_EQ('1', anImage(CONTROL_BITS + 8,0));     EXPECT_EQ('0', anImage(CONTROL_BITS + 12,0));
    EXPECT_EQ('0', anImage(CONTROL_BITS + 9,0));     EXPECT_EQ('0', anImage(CONTROL_BITS + 13,0));
    EXPECT_EQ('0', anImage(CONTROL_BITS + 10,0));    EXPECT_EQ('0', anImage(CONTROL_BITS + 14,0));
    EXPECT_EQ('1', anImage(CONTROL_BITS + 11,0));    EXPECT_EQ('0', anImage(CONTROL_BITS + 15,0));
}

TEST_F(OPAP_Suite, using2BitsGetAdjustmentEqualTo0)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 16,1);
    auto originalImage(anImage);
    auto usedBits = 2;

    originalImage.fill('0');
    anImage.fill('0');

    LSB basicPolicy(usedBits);
    basicPolicy.hide("UUU", anImage);

    EXPECT_EQ(0, opapPolicy2Bit.getAdjustment(originalImage(0,0), anImage(0,0)));
}

TEST_F(OPAP_Suite, using2BitsGetAdjustmentEqualTo2p)
{
    //to achieve: value before LSB(2) - 3, value after LSB(2) - 0, see doc for reference
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    auto originalImage(anImage);
    auto usedBits = 2;
    auto expectedAdjustment = pow(2,usedBits);
    auto carrier = 3;    //for LSB = 3 (0000 0011)
    auto secret = " "; //for LSB = 0 (0010 0000)

    originalImage.fill(carrier);
    anImage.fill(carrier);

    LSB basicPolicy(usedBits);
    basicPolicy.hide(secret, anImage);

    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 0,0), anImage(CONTROL_BITS + 0,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 1,0), anImage(CONTROL_BITS + 1,0)));
    EXPECT_EQ(0, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 2,0), anImage(CONTROL_BITS + 2,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 3,0), anImage(CONTROL_BITS + 3,0)));
}

TEST_F(OPAP_Suite, using2BitsGetAdjustmentEqualToMinus2p)
{
    //to achieve: value before LSB(2) - 0, value after LSB(2) - 3, see doc for reference
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    auto originalImage(anImage);
    auto usedBits = 2;
    auto expectedAdjustment = -pow(2,usedBits);
    auto carrier = 4;    //for LSB = 0 (0000 0100)
    auto secret = "?"; //for LSB = 3 (0011 1111)

    originalImage.fill(carrier);
    anImage.fill(carrier);

    LSB basicPolicy(usedBits);
    basicPolicy.hide(secret, anImage);

    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 0,0), anImage(CONTROL_BITS + 0,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 1,0), anImage(CONTROL_BITS + 1,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 2,0), anImage(CONTROL_BITS + 2,0)));
    EXPECT_EQ(0, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 3,0), anImage(CONTROL_BITS + 3,0)));
}

TEST_F(OPAP_Suite, using2BitsHideSecret)
{
    auto carrier = 0x0F;
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(carrier);

    auto basicLsb(anImage);
    auto originalImage(anImage);
    auto usedBits = 2;
    auto secret = "0"; // (00 11 00 00)

    LSB basicPolicy(usedBits);
    basicPolicy.hide(secret, basicLsb);
    opapPolicy2Bit.hide(secret, anImage);

    EXPECT_EQ(basicLsb(CONTROL_BITS + 0,0) + pow(2, usedBits), anImage(CONTROL_BITS + 0,0));
    EXPECT_EQ(basicLsb(CONTROL_BITS + 1,0) + pow(2, usedBits), anImage(CONTROL_BITS + 1,0));
    EXPECT_EQ(basicLsb(CONTROL_BITS + 2,0), anImage(CONTROL_BITS + 2,0));
    EXPECT_EQ(basicLsb(CONTROL_BITS + 3,0) + pow(2, usedBits), anImage(CONTROL_BITS + 3,0));

    //check if info is hidden properly
    EXPECT_EQ(0, anImage(CONTROL_BITS + 0,0) & opapPolicy2Bit.getMaskForBits(usedBits));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 1,0) & opapPolicy2Bit.getMaskForBits(usedBits));
    EXPECT_EQ(3, anImage(CONTROL_BITS + 2,0) & opapPolicy2Bit.getMaskForBits(usedBits));
    EXPECT_EQ(0, anImage(CONTROL_BITS + 3,0) & opapPolicy2Bit.getMaskForBits(usedBits));
}

TEST_F(OPAP_Suite, decodeWhatWasHiddenUsing2Bits)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    anImage.fill(0x0F);

    auto secret = "0"; // (00 11 00 00)

    opapPolicy2Bit.hide(secret, anImage);

    EXPECT_EQ(secret, opapPolicy2Bit.decode(anImage));
}

TEST_F(OPAP_Suite, decodeWhatWasHiddenUsing1Bit)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 32,1);
    anImage.fill(0x0F);

    auto secret = "012";

    opapPolicy1Bit.hide(secret, anImage);

    EXPECT_EQ(secret, opapPolicy1Bit.decode(anImage));
}

TEST_F(OPAP_Suite, adjustmentOverflowOver255)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    auto originalImage(anImage);
    auto usedBits = 2;
    auto expectedAdjustment = 0; //because of overflow
    auto carrier = 0xFF;
    auto secret = " "; //for LSB = 0 (0010 0000)

    originalImage.fill(carrier);
    anImage.fill(carrier);

    LSB basicPolicy(usedBits);
    basicPolicy.hide(secret, anImage);

    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 0,0), anImage(CONTROL_BITS + 0,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 1,0), anImage(CONTROL_BITS + 1,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 2,0), anImage(CONTROL_BITS + 2,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 3,0), anImage(CONTROL_BITS + 3,0)));
}

TEST_F(OPAP_Suite, adjustmentOverflowUnder0)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 8,1);
    auto originalImage(anImage);
    auto usedBits = 2;
    auto expectedAdjustment = 0;
    auto carrier = 0;
    auto secret = "?";

    originalImage.fill(carrier);
    anImage.fill(carrier);

    LSB basicPolicy(usedBits);
    basicPolicy.hide(secret, anImage);

    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 0,0), anImage(CONTROL_BITS + 0,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 1,0), anImage(CONTROL_BITS + 1,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 2,0), anImage(CONTROL_BITS + 2,0)));
    EXPECT_EQ(expectedAdjustment, opapPolicy2Bit.getAdjustment(originalImage(CONTROL_BITS + 3,0), anImage(CONTROL_BITS + 3,0)));
}

TEST_F(OPAP_Suite, acceptanceTestUsing1Bit)
{
    CImg<unsigned char> anImage(50,5);
    anImage.fill(0x0F);

    auto secret = "Steganography rocks!!!";

    opapPolicy1Bit.hide(secret, anImage);

    EXPECT_EQ(secret, opapPolicy1Bit.decode(anImage));
}

TEST_F(OPAP_Suite, acceptanceTestUsing2Bits)
{
    CImg<unsigned char> anImage(40,5);
    anImage.fill(0x0F);

    auto secret = "Steganography rocks!!!";

    opapPolicy2Bit.hide(secret, anImage);

    EXPECT_EQ(secret, opapPolicy2Bit.decode(anImage));
}

TEST_F(OPAP_Suite, acceptanceTestUsing4Bits)
{
    CImg<unsigned char> anImage(40,5);
    anImage.fill(0x0F);

    auto secret = "Steganography rocks!!!";

    opapPolicy4Bit.hide(secret, anImage);

    EXPECT_EQ(secret, opapPolicy4Bit.decode(anImage));
}
