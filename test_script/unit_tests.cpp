#include <gtest/gtest.h>
#include <cstdarg>
#include <vector>
#include <string>

#include "input_control/ControlParameters.h"
#include "input_control/Input_OutputProcessing.h"
#include "Exceptions.h"
#include "Filters.h"
#include "PictureInfo.h"

constexpr int BlurTestArg = 10;
constexpr int PixelTestArg = 10;
constexpr int EdgeTestArg = 30;

TEST(InputOutputTests, WrongFilePath) {
    EXPECT_THROW({ PictureInfo picture_info = InputOutputProcessing::LoadBmpFile("wrong.bmp"); }, InputDataException);
}

TEST(InputOutputTests, WrongFileHeader) {
    EXPECT_THROW(
        {
            PictureInfo picture_info =
                InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/my_data/WrongTypeFile.txt");
        },
        FileHeaderException);
}

TEST(PixelizeTests, WrongTypeArgument) {
    testing::internal::CaptureStderr();
    std::vector<std::string> strings{"./image_processor", "../tasks/image_processor/test_script/data/lenna.bmp",
                                     "data/for_testing.bmp", "-pix", "string"};

    ControlParameters bmp_pixel_zero(strings);
    bmp_pixel_zero.Control();
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_STREQ(output.c_str(), "InputDataError: -pix: Invalid type of argument\n");
}

TEST(PixelizeTests, NotPositiveArgument) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    PixelizeFilter pixel_negative(-1);
    EXPECT_THROW(pixel_negative.Apply(picture_info), InputDataException);

    PixelizeFilter pixel_zero(0);
    EXPECT_THROW(pixel_zero.Apply(picture_info), InputDataException);
}

TEST(PixelizeTests, JustPixelize) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    PixelizeFilter pixel(PixelTestArg);
    pixel.Apply(picture_info);

    PictureInfo test_picture =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/my_data/lenna_pix.bmp");
    if (picture_info.bmi_header.biWidth != test_picture.bmi_header.biWidth ||
        picture_info.bmi_header.biHeight != test_picture.bmi_header.biHeight) {
        FAIL() << "Размеры изображений не совпадают";
    }
    for (size_t y = 0; y < picture_info.bmi_header.biHeight; y++) {
        for (size_t x = 0; x < picture_info.bmi_header.biWidth; x++) {
            if (test_picture.pixels[y][x].red != picture_info.pixels[y][x].red ||
                test_picture.pixels[y][x].green != picture_info.pixels[y][x].green ||
                test_picture.pixels[y][x].blue != picture_info.pixels[y][x].blue) {
                FAIL() << "Изображения не совпадают";
            };
        }
    }
}

TEST(GaussianBlurTests, WrongTypeArgument) {
    testing::internal::CaptureStderr();
    std::vector<std::string> pixel_str{"./image_processor", "../tasks/image_processor/test_script/data/lenna.bmp",
                                       "../test_script/data/for_testing.bmp", "-blur", "string"};
    ControlParameters bmp_pixel_zero(pixel_str);
    bmp_pixel_zero.Control();
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_STREQ(output.c_str(), "InputDataError: -blur: Invalid type of argument\n");
}

TEST(GaussianBlurTests, NotPositiveArgument) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    GaussianBlurFilter blur_negative(-1);
    EXPECT_THROW(blur_negative.Apply(picture_info), InputDataException);

    GaussianBlurFilter blur_zero(0);
    EXPECT_THROW(blur_zero.Apply(picture_info), InputDataException);
}

TEST(GaussianBlurTests, JustBlur) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    GaussianBlurFilter blur(BlurTestArg);
    blur.Apply(picture_info);

    PictureInfo test_picture =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/my_data/lenna_blur.bmp");

    for (size_t y = 0; y < picture_info.bmi_header.biHeight; y++) {
        for (size_t x = 0; x < picture_info.bmi_header.biWidth; x++) {
            if (test_picture.pixels[y][x].red != picture_info.pixels[y][x].red ||
                test_picture.pixels[y][x].green != picture_info.pixels[y][x].green ||
                test_picture.pixels[y][x].blue != picture_info.pixels[y][x].blue) {
                FAIL() << "Изображения не совпадают";
            };
        }
    }
}

TEST(CropFilterTests, WrongTypeArgument) {
    testing::internal::CaptureStderr();
    std::vector<std::string> crop_str{"./image_processor",
                                      "../tasks/image_processor/test_script/data/lenna.bmp",
                                      "../tasks/image_processor/test_script/my_data/for_testing.bmp",
                                      "-crop",
                                      "string",
                                      "10"};

    ControlParameters bmp_crop_zero(crop_str);
    bmp_crop_zero.Control();
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_STREQ(output.c_str(), "InputDataError: -crop: Invalid type of argument\n");
}

TEST(CropFilterTests, NotPositiveArgument) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    CropFilter crop_negative(-1, 1);
    EXPECT_THROW(crop_negative.Apply(picture_info), InputDataException);

    GaussianBlurFilter blur_zero(0);
    EXPECT_THROW(blur_zero.Apply(picture_info), InputDataException);
}

TEST(EdgeDetectionFilterTests, WrongTypeArgument) {
    testing::internal::CaptureStderr();
    std::vector<std::string> edge_str{
        "./image_processor",
        "../tasks/image_processor/test_script/data/lenna.bmp",
        "../tasks/image_processor/test_script/my_data/for_testing.bmp",
        "-edge",
        "string",
    };

    ControlParameters bmp_crop_zero(edge_str);
    bmp_crop_zero.Control();
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_STREQ(output.c_str(), "InputDataError: -edge: Invalid type of argument\n");
}

TEST(EdgeDetectionFilterTests, JustEdge) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    EdgeDetectionFilter edge(EdgeTestArg);
    edge.Apply(picture_info);

    PictureInfo test_picture =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/my_data/lenna_edge.bmp");

    for (size_t y = 0; y < picture_info.bmi_header.biHeight; y++) {
        for (size_t x = 0; x < picture_info.bmi_header.biWidth; x++) {
            if (test_picture.pixels[y][x].red != picture_info.pixels[y][x].red ||
                test_picture.pixels[y][x].green != picture_info.pixels[y][x].green ||
                test_picture.pixels[y][x].blue != picture_info.pixels[y][x].blue) {
                FAIL() << "Изображения не совпадают";
            };
        }
    }
}

TEST(NegativeFilterTests, JustNeg) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    NegativeFilter neg;
    neg.Apply(picture_info);

    PictureInfo test_picture =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna_neg.bmp");

    for (size_t y = 0; y < picture_info.bmi_header.biHeight; y++) {
        for (size_t x = 0; x < picture_info.bmi_header.biWidth; x++) {
            if (test_picture.pixels[y][x].red != picture_info.pixels[y][x].red ||
                test_picture.pixels[y][x].green != picture_info.pixels[y][x].green ||
                test_picture.pixels[y][x].blue != picture_info.pixels[y][x].blue) {
                FAIL() << "Изображения не совпадают";
            };
        }
    }
}

TEST(GrayScaleFilterTests, JustGray) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    GrayScaleFilter gs;
    gs.Apply(picture_info);

    PictureInfo test_picture =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna_gs.bmp");

    for (size_t y = 0; y < picture_info.bmi_header.biHeight; y++) {
        for (size_t x = 0; x < picture_info.bmi_header.biWidth; x++) {
            if (test_picture.pixels[y][x].red != picture_info.pixels[y][x].red ||
                test_picture.pixels[y][x].green != picture_info.pixels[y][x].green ||
                test_picture.pixels[y][x].blue != picture_info.pixels[y][x].blue) {
                FAIL() << "Изображения не совпадают";
            };
        }
    }
}

TEST(SharpeningFilterTests, JustSharp) {
    PictureInfo picture_info =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna.bmp");
    SharpeningFilter sharp;
    sharp.Apply(picture_info);

    PictureInfo test_picture =
        InputOutputProcessing::LoadBmpFile("../tasks/image_processor/test_script/data/lenna_sharp.bmp");

    for (size_t y = 0; y < picture_info.bmi_header.biHeight; y++) {
        for (size_t x = 0; x < picture_info.bmi_header.biWidth; x++) {
            if (test_picture.pixels[y][x].red != picture_info.pixels[y][x].red ||
                test_picture.pixels[y][x].green != picture_info.pixels[y][x].green ||
                test_picture.pixels[y][x].blue != picture_info.pixels[y][x].blue) {
                FAIL() << "Изображения не совпадают";
            };
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
