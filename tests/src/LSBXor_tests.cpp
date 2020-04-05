#include "UnitTestsCommon.h"
#include "LSBXor.h"

class LSBXor_Suite : public ::testing::Test
{
protected:
    LSBXor_Suite()
    {
    }
    virtual ~LSBXor_Suite()
    {
    }
    LSBXor xorLsbPolicy;
    BitsOperations bitOperation;
};

TEST_F(LSBXor_Suite, getX1X2FromImage)
{
    CImg<unsigned char> anImage(1,1);
    auto pixelValue = 234;
    anImage.fill(pixelValue);
    //234 = (bit) 11101010
    //X1 = 1
    //X2 = 0
    //get X1 X2
//    from external image (for retrieving data)
    auto x1 = xorLsbPolicy.getX1(anImage(0,0));
    auto x2 = xorLsbPolicy.getX2(anImage(0,0));

    EXPECT_EQ(x1, 1);
    EXPECT_EQ(x2, 0);
}

TEST_F(LSBXor_Suite, X1EqualsX2HideBit0NoChangeRequired)
{
    CImg<unsigned char> anImage(16, 1);
    auto pixelValue = 0xFF;
    anImage.fill(pixelValue);
                                //carrier is 0xFF -   11111111
                                //X1 == X2 -> if we want to hide 0 we do not need to change pixel LSB
    std::string message = "0";  //'0' is 48 in int so 00110000 in binary
    xorLsbPolicy.hide(message, anImage);

    auto expectedLsbBit = 1;
    auto lsb = anImage(0,0) & bitOperation.getMaskForBits(1);

    EXPECT_EQ(expectedLsbBit, lsb);
}

TEST_F(LSBXor_Suite, X1EqualsX2HideBit1ChangeOnLsbRequired)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 16,1);
    auto pixelValue = 0xFF;
    anImage.fill(pixelValue);
                                //carrier is 0xFF -   11111111
                                //X1 == X2 -> if we want to hide 1 we need to change pixel LSB
    std::string message = "1";  //'1' is 49 in int so 00110001 in binary
    xorLsbPolicy.hide(message, anImage);

    auto expectedLsbBit = 0;
    auto lsb = anImage(CONTROL_BITS + 0,0) & bitOperation.getMaskForBits(1);

    EXPECT_EQ(expectedLsbBit, lsb);
}


TEST_F(LSBXor_Suite, X1DiffersFromX2HideBit0ChangeOnLsbRequired)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 16,1);
    auto pixelValue = 0xFE;
    anImage.fill(pixelValue);
                                //carrier is 0xFE -   11111110
                                //X1 != X2 -> if we want to hide 0 we need to change pixel LSB
    std::string message = "0";  //'0' is 48 in int so 00110000 in binary
    xorLsbPolicy.hide(message, anImage);

    auto expectedLsbBit = 1;
    auto lsb = anImage(CONTROL_BITS + 0,0) & bitOperation.getMaskForBits(1);

    EXPECT_EQ(expectedLsbBit, lsb);
}

TEST_F(LSBXor_Suite, X1EqualsX2HideBit1NoChangeRequired)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 16,1);
    auto pixelValue = 0xFE;
    anImage.fill(pixelValue);
                                //carrier is 0xFE -   11111110
                                //X1 != X2 -> if we want to hide 1 we do not need to change pixel LSB
    std::string message = "1";  //'1' is 49 in int so 00110001 in binary
    xorLsbPolicy.hide(message, anImage);

    auto expectedLsbBit = 0;
    auto lsb = anImage(CONTROL_BITS + 0,0) & bitOperation.getMaskForBits(1);

    EXPECT_EQ(expectedLsbBit, lsb);
}


TEST_F(LSBXor_Suite, HideCharInImageWhenX1EqualsX2NoTab)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 16,1);
    auto pixelValue = 0xFF;
    anImage.fill(pixelValue);
                                //carrier is 0xFF -   11111111
                                //X1 == X2 -> if we want to hide 1 we do need to change pixel LSB (otherwise change it)
    std::string message = "1";  //'1' is 49 in int so 00110001 in binary
                                //'\t' will be added to message to know where the end is
    xorLsbPolicy.hide(message, anImage);

    EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 0,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 4,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 8,0));  EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 12,0));
    EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 1,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 5,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 9,0));  EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 13,0));
    EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 2,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 6,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 10,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 14,0));
    EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 3,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 7,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 11,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 15,0));
}

TEST_F(LSBXor_Suite, HideCharInImageWhenX1EqualsX2WithTab)
{
    CImg<unsigned char> anImage(CONTROL_BITS + 16,1);
    auto pixelValue = 0xFF;
    anImage.fill(pixelValue);
                                //carrier is 0xFF -   11111111
                                //X1 == X2 -> if we want to hide 1 we do  need to change pixel LSB (otherwise change it)
    std::string message = "1\t";  //'1' is 49 in int so 00110001 in binary
                                  //'\t' is 00001001
    xorLsbPolicy.hide(message, anImage);

    //not correctly distributed, becasuse anImage(0, 1) means column 0 and row 1
    EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 0,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 4,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 8,0));  EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 12,0));
    EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 1,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 5,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 9,0));  EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 13,0));
    EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 2,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 6,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 10,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 14,0));
    EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 3,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 7,0)); EXPECT_EQ(0xFE, anImage(CONTROL_BITS + 11,0)); EXPECT_EQ(0xFF, anImage(CONTROL_BITS + 15,0));
}

TEST_F(LSBXor_Suite, DecodePreviousleEncodedByteCarierFF)
{
    //prepare carrier image
    CImg<unsigned char> carrierImage(CONTROL_BITS + 16,1);
    auto pixelValue = 0xFF;
    carrierImage.fill(pixelValue);

    //prepare stego image
    auto secretMessage = "9";
    xorLsbPolicy.hide(secretMessage, carrierImage);

    //decode message from stego image
    auto decodedMessage = xorLsbPolicy.decode(carrierImage);

    EXPECT_EQ(secretMessage, decodedMessage);
}

TEST_F(LSBXor_Suite, DecodePreviousleEncodedByteUsingCarrier00)
{
    //prepare carrier image
    CImg<unsigned char> carrierImage(CONTROL_BITS + 16,1);
    auto pixelValue = 0x00;
    carrierImage.fill(pixelValue);

    //prepare stego image
    auto secretMessage = "9";
    xorLsbPolicy.hide(secretMessage, carrierImage);

    //decode message from stego image
    auto decodedMessage = xorLsbPolicy.decode(carrierImage);

    EXPECT_EQ(secretMessage, decodedMessage);
}

TEST_F(LSBXor_Suite, DecodePreviousleEncodedMessageUsingCarrierF4)
{
    //prepare carrier image
    CImg<unsigned char> carrierImage(20,20);
    auto pixelValue = 0xF4;
    carrierImage.fill(pixelValue);

    //prepare stego image
    auto secretMessage = "Steganography rocks!!!";
    xorLsbPolicy.hide(secretMessage, carrierImage);

    //decode message from stego image
    auto decodedMessage = xorLsbPolicy.decode(carrierImage);

    EXPECT_EQ(secretMessage, decodedMessage);
}
