#include "UnitTestsCommon.h"
#include "MethodHandler.h"

class MethodHandlerFixture : public ::testing::Test, public ::testing::WithParamInterface<int>
{
protected:
    MethodHandlerFixture()
    {
    }
    virtual ~MethodHandlerFixture()
    {
    }
    MethodHandler methodHandler;
};

TEST_F(MethodHandlerFixture, getLsbMethodWhenBitsEqualTo0)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    anImage(0,0) = 0;
    anImage(1,0) = 0;

    EXPECT_EQ(MethodHandler::TYPE_BASIC_LSB, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, getLsbXorMethodWhenBitsEqualTo1)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    anImage(0,0) = 0;
    anImage(1,0) = 1;

    EXPECT_EQ(MethodHandler::TYPE_XOR_LSB, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, getInvertedLsbMethodWhenBitsEqualTo2)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    anImage(0,0) = 1;
    anImage(1,0) = 0;

    EXPECT_EQ(MethodHandler::TYPE_INVERTED_LSB, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, getInvertedLsbMethodWhenLSBBitsEqualTo2)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    anImage(0,0) = 37;
    anImage(1,0) = 36;

    EXPECT_EQ(MethodHandler::TYPE_INVERTED_LSB, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, getOpapMethodWhenBitsEqualTo3)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    anImage(0,0) = 1;
    anImage(1,0) = 1;

    EXPECT_EQ(MethodHandler::TYPE_OPAP, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, getOpapMethodWhenLSBBitsEqualTo3)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    anImage(0,0) = 255;
    anImage(1,0) = 255;

    EXPECT_EQ(MethodHandler::TYPE_OPAP, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, setLsbMethod)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    methodHandler.setMethodType(anImage, MethodHandler::TYPE_BASIC_LSB);

    EXPECT_EQ(MethodHandler::TYPE_BASIC_LSB, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, setLsbXorMethod)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    methodHandler.setMethodType(anImage, MethodHandler::TYPE_XOR_LSB);

    EXPECT_EQ(MethodHandler::TYPE_XOR_LSB, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, setLsbInvMethod)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    methodHandler.setMethodType(anImage, MethodHandler::TYPE_INVERTED_LSB);

    EXPECT_EQ(MethodHandler::TYPE_INVERTED_LSB, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, setOpapMethod)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);

    methodHandler.setMethodType(anImage, MethodHandler::TYPE_OPAP);

    EXPECT_EQ(MethodHandler::TYPE_OPAP, methodHandler.getMethodType(anImage));
}

TEST_F(MethodHandlerFixture, setMethodCheckImageNotChangeedTooMuch)
{
    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);
    anImage.fill(0xFF);
    methodHandler.setMethodType(anImage, MethodHandler::TYPE_BASIC_LSB);

    EXPECT_EQ(MethodHandler::TYPE_BASIC_LSB, methodHandler.getMethodType(anImage));
    EXPECT_EQ(254, anImage(0,0));
    EXPECT_EQ(254, anImage(0,0));
}

INSTANTIATE_TEST_CASE_P(usedBitsCode,
                        MethodHandlerFixture,
                        ::testing::Values(MethodHandler::USED_BITS_1,
                                          MethodHandler::USED_BITS_2,
                                          MethodHandler::USED_BITS_4,
                                          MethodHandler::USED_BITS_8));

TEST_P(MethodHandlerFixture, usedBitsCode)
{
    auto pixelValue = 0x01;
    auto usedBitsCode = GetParam();

    CImg<unsigned char> anImage(CONTROL_BITS + CHAR_SIZE,1);
    anImage.fill(pixelValue);
    methodHandler.setUsedBitsCode(anImage, usedBitsCode);

    auto expectedMsbPixel = BitsOperations::getBitAt(usedBitsCode, 2);
    auto expectedLsbPixel = BitsOperations::getBitAt(usedBitsCode, 1);

    EXPECT_EQ(usedBitsCode, methodHandler.getUsedBitsCode(anImage));
    EXPECT_EQ(expectedMsbPixel, anImage(2,0));
    EXPECT_EQ(expectedLsbPixel, anImage(3,0));
}

TEST(MethodHandler, CheckMethodAllZeroes)
{
    CImg<unsigned char> anImage(15,15);
    anImage.fill(0);

    EXPECT_EQ(MethodHandler::TYPE_BASIC_LSB, MethodHandler::getMethodType(anImage));
}

TEST(MethodHandler, CheckMethodAllOnes)
{
    CImg<unsigned char> anImage(15,15);
    anImage.fill(1);

    EXPECT_EQ(MethodHandler::TYPE_OPAP, MethodHandler::getMethodType(anImage));
}

