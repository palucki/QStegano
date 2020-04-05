#include "UnitTestsCommon.h"
#include "ImageContext.h"

void printImage(const CImg<unsigned char>& anImage)
{
    cimg_forC(anImage, channel)
    {
        std::cout << "Channel: " << channel << "\n";
        cimg_forX(anImage, x)
        {
            cimg_forY(anImage, y)
            {
                std::cout << (int) anImage(x,y,0,channel) << " ";
            }
            std::cout << "\n";
        }
    }
    std::cout.flush();
}

bool areEqual(CImg<unsigned char> lhs, CImg<unsigned char> rhs)
{    
    if(rhs.height() != lhs.height() ||  rhs.width() != lhs.width())
    {
        std::cout << "Sizes differ!" << std::endl;
        return false;
    } 
        
    cimg_forXY(rhs,x,y)
    {
        for(int c = 0; c < rhs.spectrum(); c++)
        {
            if(rhs(x,y,0,c) != lhs(x,y,0,c))
            {
                std::cout << "Pixel [" << x << "," << y <<"] differs at color " << c << " !" << std::endl;
                std::cout << "lhs = " << lhs(x,y,0,c) << std::endl;
                std::cout << "rhs = " << rhs(x,y,0,c) << std::endl; 
                return false;
            }
        }
    }
    
    return true;
}

TEST(ImagesAreEqual, Basic2x2ImagesAreEqual)
{
    CImg<unsigned char> emptyImage(2,2,1,1,0);
    CImg<unsigned char> alsoEmpty(2,2,1,1,0);
    CImg<unsigned char> anImage(2,2);
    
    //EXPECT_FALSE(anImage == emptyImage);
    EXPECT_EQ(alsoEmpty, emptyImage);
}


TEST(MaskFromBits, CheckAllNumbersOfBitsFrom1To7)
{
    BitsOperations bitOperation;

    EXPECT_EQ(bitOperation.getMaskForBits(0), 0x00);
    EXPECT_EQ(bitOperation.getMaskForBits(1), 0x01);
    EXPECT_EQ(bitOperation.getMaskForBits(2), 0x03);
    EXPECT_EQ(bitOperation.getMaskForBits(3), 0x07);
    EXPECT_EQ(bitOperation.getMaskForBits(4), 0x0F);
    EXPECT_EQ(bitOperation.getMaskForBits(5), 0x1F);
    EXPECT_EQ(bitOperation.getMaskForBits(6), 0x3F);
    EXPECT_EQ(bitOperation.getMaskForBits(7), 0x7F);
    EXPECT_EQ(bitOperation.getMaskForBits(8), 0xFF);
}

TEST(CImgIterator, BasicIterators)
{
    CImg<unsigned char> anImage(CImg<unsigned char>::identity_matrix(2));

    auto it = anImage.begin();
    auto callCountForIdentity = 0;

    for(auto i = 0; i<7; i++)
    {
        if(it != anImage.end()) {
            callCountForIdentity++;
            it++;
        }
    }

    EXPECT_EQ(callCountForIdentity, 4);

    CImg<unsigned char> rgbImage(2, 2, 1, 3);

//  In RGB image iterator loops trhough each channel pixel, and then changes channel

    it = rgbImage.begin();
    auto callCountForRGB = 0;

    for(auto i = 0; i<20; i++)
    {
        if(it != rgbImage.end()) {
            callCountForRGB++;
            it++;
        }
    }

    EXPECT_EQ(callCountForRGB, 12);
}

TEST(CImgIterator, WhenSecretTooLongForImageDontProcessImage)
{
    CImg<unsigned char> originalImage(CImg<unsigned char>::identity_matrix(3));
    auto processedImage(originalImage);

    std::string secretMessage = "ab"; //2 characters needs 16 bits, image has only 9
    ImageContext context(processedImage);
    context.hideUsingBasicLsb(secretMessage);

    EXPECT_EQ(originalImage, processedImage);
}

TEST(ImageContext, CheckIndexing)
{
    CImg<unsigned char> originalImage(CImg<unsigned char>::identity_matrix(3));
    originalImage(0,1) = 1; //row 1, column 0

//    auto imageIterator = originalImage.begin();

//    for(auto imageElement : originalImage)
//    {
//        //UNCOMMENT TO TEST MANUALLY
////        std::cout << "Manual iterator " << (int)*imageIterator++ << std::endl;
////        std::cout << "Range for loop  " << (int)imageElement << std::endl;
//    }

}

TEST(ImageContext, countNumberOfPixelsDifferentBetweenImages)
{
    CImg<unsigned char> anImage(CImg<unsigned char>::identity_matrix(3));

    ImageContext context(anImage);

    anImage(0,0) = 0xFF;
    anImage(1,1) = 0x00;

    EXPECT_EQ(2, context.getNumberOfDifferentPixels(anImage));
}

TEST(ImageContext, changeOnImageDoesNotAffectItsCopy)
{
    CImg<unsigned char> anImage(CImg<unsigned char>::identity_matrix(3));
    auto copy = anImage;

    anImage(0,0) = 0xFF;

    EXPECT_NE(anImage, copy);
}

TEST(ImageContext,WhenGotWhitePixelLSBReturnsEmptyImage)
{
    unsigned char whiteInRGB = 255;
    unsigned char whiteInBlackAndWhite = whiteInRGB & 0x01;

    CImg<unsigned char> anImage(1,1,1,1,whiteInRGB);
    CImg<unsigned char> lsb(1,1,1,1,whiteInBlackAndWhite);

    ImageContext context(anImage);

    EXPECT_EQ(context.getImageLSB(anImage, 0,1), lsb);
}

TEST(ImageContext, WhenGotBlackPixelLSBReturnsBlackImage)
{
    unsigned char blackInRGB = 0;
    unsigned char blackInBlackAndWhite = blackInRGB & 0x01;

    CImg<unsigned char> anImage(1,1,1,1,blackInRGB);
    CImg<unsigned char> lsb(1,1,1,1,blackInBlackAndWhite);

    ImageContext context(anImage);

    EXPECT_EQ(context.getImageLSB(anImage, 0,1), lsb);
}

TEST(ImageContext, WhenGot3x3ImageReturnItsLSBImage)
{
    CImg<unsigned char> anImage(CImg<unsigned char>::identity_matrix(3));
    CImg<unsigned char> lsb(3,3,1,1,0);

    cimg_forX(lsb, x)
    {
        lsb(x,x,0,0) = 0x01;
    }

    ImageContext context(anImage);

    EXPECT_EQ(context.getImageLSB(anImage, 0,1), lsb);
}


TEST(ImageContext, GetImageLSBForSpecifiedNumberOfBits)
{
    CImg<unsigned char> anImage(CImg<unsigned char>::identity_matrix(5)),
                        lsb3Bits(CImg<unsigned char>::identity_matrix(5)),
                        lsb7Bits(CImg<unsigned char>::identity_matrix(5));

    auto channel = 0;

    cimg_forX(lsb3Bits, x)
    {
        anImage(x,x) = 0xFF;
        lsb3Bits(x,x) = 0x07;
        lsb7Bits(x,x) = 0x7F;
    }

    ImageContext context(anImage);

    auto numberOfBits = 3;
    EXPECT_EQ(context.getImageLSB(anImage, channel, numberOfBits), lsb3Bits);

    numberOfBits = 7;
    EXPECT_EQ(context.getImageLSB(anImage, channel, numberOfBits), lsb7Bits);
}


TEST(ImageContext, GetImageLSBForSpecifiedChannel)
{
    CImg<unsigned char> anImage(CImg<unsigned char>::identity_matrix(3));
    ImageContext context(anImage);

    unsigned char channel = 0;
    EXPECT_EQ(context.getImageLSB(anImage, channel, 1), anImage);
}

TEST(ImageContext, GivenTooHighChannelNumberReturnIdentityMatrix)
{
    CImg<unsigned char> anImage(CImg<unsigned char>::identity_matrix(3));
    ImageContext context(anImage);

    unsigned char channel = 1;
    EXPECT_FALSE(context.getImageLSB(anImage, channel, 1) == anImage);
}

TEST(LSB, CheckThatLsbWithContextDoNotModifiesOriginalImage)
{
    CImg<unsigned char> anImage(15,15);
    anImage.fill(0);
    ImageContext context(anImage);

    auto originalImage = anImage;
    auto usedBits = 1;
    context.hideUsingBasicLsb("acdef", usedBits);

    auto steganoImage = context.getBasicLsb();
    auto lsb = context.getImageLSB(steganoImage, 0, usedBits);

    auto diffBetweenLsbAndImage = 0;
    auto diffBetweenImageAndOriginalImage = 0;
    cimg_forXY(anImage, x, y)
    {
        if(anImage(x,y) != originalImage(x,y))
            diffBetweenImageAndOriginalImage++;
        if(anImage(x,y) != lsb(x,y))
            diffBetweenLsbAndImage++;
    }

    EXPECT_EQ(0, diffBetweenImageAndOriginalImage);
    EXPECT_NE(0, diffBetweenLsbAndImage);
}

TEST(ImageContext, WhenImageDoNotContainMessageReturn_CRC_CHECK_FAILED)
{
    CImg<unsigned char> anImage(15,15);
    anImage.fill(0);
    ImageContext context(anImage);

    auto message = context.decode(anImage);

    EXPECT_EQ(CRC_FAILED_MSG, message);
}
