#include "UnitTestsCommon.h"
#include "LSB.h"

class LSB_Suite : public ::testing::Test
{
protected:
    LSB_Suite() : basicLsbPolicy1Bit(LSB(1)),
                  basicLsbPolicy2Bit(LSB(2)),
                  basicLsbPolicy4Bit(LSB(4)),
                  basicLsbPolicy8Bit(LSB(8))
    {
    }
    virtual ~LSB_Suite()
    {
    }
    LSB basicLsbPolicy1Bit;
    LSB basicLsbPolicy2Bit;
    LSB basicLsbPolicy4Bit;
    LSB basicLsbPolicy8Bit;
};

TEST_F(LSB_Suite, Hide1CharIn3x3ImageUsing1Bit)
{
    CImg<unsigned char> anImage(CHAR_SIZE + ENDING_TAB_SIZE + CONTROL_BITS, 1);
    anImage.fill(0);

    basicLsbPolicy1Bit.hide("1", anImage);
    //'1' is 49 in int so 00110001 in binary

    EXPECT_EQ(anImage(CONTROL_BITS + 0,0), 1);  EXPECT_EQ(anImage(CONTROL_BITS + 3,0), 0);  EXPECT_EQ(anImage(CONTROL_BITS + 6,0), 0);
    EXPECT_EQ(anImage(CONTROL_BITS + 1,0), 0);  EXPECT_EQ(anImage(CONTROL_BITS + 4,0), 1);  EXPECT_EQ(anImage(CONTROL_BITS + 7,0), 0);
    EXPECT_EQ(anImage(CONTROL_BITS + 2,0), 0);  EXPECT_EQ(anImage(CONTROL_BITS + 5,0), 1);
}

TEST_F(LSB_Suite, Hide2CharsIn4x4ImageUsing1Bit)
{
    CImg<unsigned char> anImage(2*CHAR_SIZE + ENDING_TAB_SIZE + CONTROL_BITS, 1);
    anImage.fill(0);

    basicLsbPolicy1Bit.hide("ab", anImage);
    //'a' is 97 in int so 0110 0001 in binary
    //'b' is 98 in int so 0110 0010 in binary

//  X - - -
//  - X X X
//  - X - X
//  - - - -

    EXPECT_EQ(anImage(CONTROL_BITS + 0,0), 1); EXPECT_EQ(anImage(CONTROL_BITS + 4,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 8,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 12,0), 0);
    EXPECT_EQ(anImage(CONTROL_BITS + 1,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 5,0), 1); EXPECT_EQ(anImage(CONTROL_BITS + 9,0), 1); EXPECT_EQ(anImage(CONTROL_BITS + 13,0), 1);
    EXPECT_EQ(anImage(CONTROL_BITS + 2,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 6,0), 1); EXPECT_EQ(anImage(CONTROL_BITS + 10,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 14,0), 1);
    EXPECT_EQ(anImage(CONTROL_BITS + 3,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 7,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 11,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 15,0), 0);
}

TEST_F(LSB_Suite, Hide2CharactersIn3x3ImageUsing2Bits)
{
    CImg<unsigned char> anImage(CHAR_SIZE + ENDING_TAB_SIZE + CONTROL_BITS, 1);
    anImage.fill(0x03);

    basicLsbPolicy2Bit.hide("ab", anImage);
    //'a' is 97 in int so 0110 0001 in binary
    //'b' is 98 in int so 0110 0010 in binary

    //order is LSB to MSB, iterators goes through rows in each column and then through columns
    //we expect 01 00 10 01 10 00 10 01

    EXPECT_EQ(anImage(CONTROL_BITS + 0,0), 1); EXPECT_EQ(anImage(CONTROL_BITS + 3,0), 1); EXPECT_EQ(anImage(CONTROL_BITS + 6,0), 2);
    EXPECT_EQ(anImage(CONTROL_BITS + 1,0), 0); EXPECT_EQ(anImage(CONTROL_BITS + 4,0), 2); EXPECT_EQ(anImage(CONTROL_BITS + 7,0), 1);
    EXPECT_EQ(anImage(CONTROL_BITS + 2,0), 2); EXPECT_EQ(anImage(CONTROL_BITS + 5,0), 0);
}

TEST_F(LSB_Suite, Hide4CharactersIn3x3ImageUsing8Bits)
{
    CImg<unsigned char> anImage(CHAR_SIZE + ENDING_TAB_SIZE + CONTROL_BITS, 1);
    anImage.fill(0x0F);

    basicLsbPolicy4Bit.hide("abcd", anImage);
    //'a' is  97 in int so 0110 0001 in binary
    //'b' is  98 in int so 0110 0010 in binary
    //'c' is  99 in int so 0110 0011 in binary
    //'d' is 100 in int so 0110 0100 in binary

    //order is LSB to MSB, iterators goes through rows in each column and then through columns
    //we expect 0001 0110 - 0010 0110 - 0011 0110 - 0100 0110

    EXPECT_EQ(anImage(CONTROL_BITS + 0,0), 1); EXPECT_EQ(anImage(CONTROL_BITS + 3,0), 6); EXPECT_EQ(anImage(CONTROL_BITS + 6,0), 4);
    EXPECT_EQ(anImage(CONTROL_BITS + 1,0), 6); EXPECT_EQ(anImage(CONTROL_BITS + 4,0), 3); EXPECT_EQ(anImage(CONTROL_BITS + 7,0), 6);
    EXPECT_EQ(anImage(CONTROL_BITS + 2,0), 2); EXPECT_EQ(anImage(CONTROL_BITS + 5,0), 6);
}

TEST_F(LSB_Suite, Hide8CharactersIn3x3ImageUsing4Bits)
{
    CImg<unsigned char> anImage(CHAR_SIZE + ENDING_TAB_SIZE + CONTROL_BITS, 1);
    anImage.fill(0xFF);

    basicLsbPolicy8Bit.hide("abcdabcd", anImage);
    //'a' is  97 in int so 0110 0001 in binary
    //'b' is  98 in int so 0110 0010 in binary
    //'c' is  99 in int so 0110 0011 in binary
    //'d' is 100 in int so 0110 0100 in binary

    //order is LSB to MSB, iterators goes through rows in each column and then through columns
    //we expect 00010110 - 00100110 - 00110110 - 01000110 - 00010110 - 00100110 - 00110110 - 01000110

    EXPECT_EQ(anImage(CONTROL_BITS + 0,0), 'a'); EXPECT_EQ(anImage(CONTROL_BITS + 3,0), 'd'); EXPECT_EQ(anImage(CONTROL_BITS + 6,0), 'c');
    EXPECT_EQ(anImage(CONTROL_BITS + 1,0), 'b'); EXPECT_EQ(anImage(CONTROL_BITS + 4,0), 'a'); EXPECT_EQ(anImage(CONTROL_BITS + 7,0), 'd');
    EXPECT_EQ(anImage(CONTROL_BITS + 2,0), 'c'); EXPECT_EQ(anImage(CONTROL_BITS + 5,0), 'b'); EXPECT_EQ(anImage(CONTROL_BITS + 8,0), '\t');
}

TEST_F(LSB_Suite, DecodeCharFromLsbUsing2Bits)
{
    CImg<unsigned char> anImage(30,30);
    anImage.fill(0xFF);
    std::string secretMessage = "I"; //0110 0001 in ASCII

    basicLsbPolicy2Bit.hide(secretMessage, anImage);

    std::string result = basicLsbPolicy2Bit.decode(anImage);

    EXPECT_EQ(secretMessage, result);
}

TEST_F(LSB_Suite, DecodeTextWithoutTabFromLsbUsing1Bit)
{
    CImg<unsigned char> anImage(30,30);
    anImage.fill(0xFF);
    std::string secretMessage = "Is this a real life?";

    basicLsbPolicy1Bit.hide(secretMessage, anImage);
    std::string result = basicLsbPolicy1Bit.decode(anImage);

    EXPECT_EQ(secretMessage, result);
}

TEST_F(LSB_Suite, DecodeTextWithTabFromLsbUsing1Bit)
{
    CImg<unsigned char> anImage(30,30);
    anImage.fill(0xFF);

    std::string secretMessage = "Is this a real life?\tQWERTY";
    std::string decodedMessage = "Is this a real life?";

    basicLsbPolicy1Bit.hide(secretMessage, anImage);
    std::string result = basicLsbPolicy1Bit.decode(anImage);

    EXPECT_EQ(decodedMessage, result);
}

TEST_F(LSB_Suite, acceptanceTest)
{
    CImg<unsigned char> anImage(15,15);
    anImage.fill(0xDA);
    auto secretMessage = "Rock & roll";

    basicLsbPolicy1Bit.hide(secretMessage, anImage);
    std::string result = basicLsbPolicy1Bit.decode(anImage);

    EXPECT_EQ(secretMessage, result);
}
