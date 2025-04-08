#include <cmath>
#include <algorithm>

#include "Exceptions.h"
#include "Filters.h"

void NegativeFilter::Apply(PictureInfo &picture_info) {
    for (LONG y = 0; y < picture_info.bmi_header.biHeight; ++y) {
        for (LONG x = 0; x < picture_info.bmi_header.biWidth; ++x) {
            picture_info.pixels[y][x].red =
                static_cast<BYTE>(std::clamp(MaxColorValint - picture_info.pixels[y][x].red, 0, MaxColorValint));
            picture_info.pixels[y][x].green =
                static_cast<BYTE>(std::clamp(MaxColorValint - picture_info.pixels[y][x].green, 0, MaxColorValint));
            picture_info.pixels[y][x].blue =
                static_cast<BYTE>(std::clamp(MaxColorValint - picture_info.pixels[y][x].blue, 0, MaxColorValint));
        }
    }
}

void GrayScaleFilter::Apply(PictureInfo &picture_info) {
    for (LONG y = 0; y < picture_info.bmi_header.biHeight; ++y) {
        for (LONG x = 0; x < picture_info.bmi_header.biWidth; ++x) {
            double gray_value = GrayRed * picture_info.pixels[y][x].red + GrayGreen * picture_info.pixels[y][x].green +
                                GrayBlue * picture_info.pixels[y][x].blue;

            picture_info.pixels[y][x].red = static_cast<BYTE>(std::clamp(gray_value, 0.0, MaxColorValdouble));
            picture_info.pixels[y][x].green = picture_info.pixels[y][x].red;
            picture_info.pixels[y][x].blue = picture_info.pixels[y][x].red;
        }
    }
}

void EdgeDetectionFilter::Apply(PictureInfo &picture_info) {
    GrayScaleFilter::Apply(picture_info);
    std::vector<std::vector<Pixel> > image_copy = picture_info.pixels;

    for (LONG y = 0; y < picture_info.bmi_header.biHeight; ++y) {
        for (LONG x = 0; x < picture_info.bmi_header.biWidth; ++x) {
            double color = 0;

            const int kernel[3][3] = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    Pixel neighbor = picture_info.CheckingBorders(x - 1 + row, y - 1 + col, picture_info);
                    color += static_cast<double>(neighbor.red * ((row + 1) * 3 + col + 1)) * kernel[row][col];
                }
            }
            color = std::clamp(color, 0.0, MaxColorValdouble);

            if (color > threshold_) {
                image_copy[y][x].red = image_copy[y][x].green = image_copy[y][x].blue =
                    static_cast<BYTE>(MaxColorValint);
            } else {
                image_copy[y][x].red = image_copy[y][x].green = image_copy[y][x].blue = 0;
            }
        }
    }
    picture_info.pixels = image_copy;
}

void SharpeningFilter::Apply(PictureInfo &picture_info) {
    std::vector<std::vector<Pixel> > image = picture_info.pixels;
    std::vector<std::vector<Pixel> > image_copy = picture_info.pixels;

    for (LONG y = 0; y < picture_info.bmi_header.biHeight; ++y) {
        for (LONG x = 0; x < picture_info.bmi_header.biWidth; ++x) {
            int red = 0;
            int green = 0;
            int blue = 0;

            const int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    Pixel neighbor = picture_info.CheckingBorders(x - 1 + row, y - 1 + col, picture_info);
                    red += neighbor.red * kernel[row][col];
                    green += neighbor.green * kernel[row][col];
                    blue += neighbor.blue * kernel[row][col];
                }
            }
            image_copy[y][x].red = static_cast<BYTE>(std::clamp(red, 0, MaxColorValint));
            image_copy[y][x].green = static_cast<BYTE>(std::clamp(green, 0, MaxColorValint));
            image_copy[y][x].blue = static_cast<BYTE>(std::clamp(blue, 0, MaxColorValint));
        }
    }
    picture_info.pixels = image_copy;
}

void CropFilter::Apply(PictureInfo &picture_info) {
    if (y_crop_ <= 0 || x_crop_ <= 0) {
        throw InputDataException("Maybe you wanna delete image?");
    }

    if (y_crop_ < picture_info.bmi_header.biHeight) {
        std::reverse(picture_info.pixels.begin(), picture_info.pixels.end());
        picture_info.pixels.resize(y_crop_);
        picture_info.bmi_header.biHeight = y_crop_;
        std::reverse(picture_info.pixels.begin(), picture_info.pixels.end());
    }
    if (x_crop_ < picture_info.bmi_header.biWidth) {
        for (std::vector<Pixel> &row : picture_info.pixels) {
            row.resize(x_crop_);
        }
        picture_info.bmi_header.biWidth = x_crop_;
    }
}

void GaussianBlurFilter::CreateGaussianKernel() {
    int center = kernel_size_ / 2;
    kernel_ = std::vector<double>(kernel_size_, 0.0);
    double summat = 0.0;
    for (int i = 0; i < kernel_size_; ++i) {
        int dx = i - center;
        double value = exp(-(dx * dx / (2 * sigma_ * sigma_)));
        kernel_[i] = value;
        summat += value;
    }

    for (int i = 0; i < kernel_size_; ++i) {
        kernel_[i] /= summat;
    }
}

void GaussianBlurFilter::Apply(PictureInfo &picture_info) {
    if (sigma_ <= 0) {
        throw InputDataException("sigma must be positive");
    }
    CreateGaussianKernel();

    double new_blue = 0.0;
    double new_green = 0.0;
    double new_red = 0.0;
    int center = kernel_size_ / 2;
    std::vector<std::vector<Pixel> > image_copy = picture_info.pixels;
    for (LONG y = 0; y < picture_info.bmi_header.biHeight; ++y) {
        for (LONG x = 0; x < picture_info.bmi_header.biWidth; ++x) {
            new_blue = 0.0;
            new_green = 0.0;
            new_red = 0.0;
            for (int ky = 0; ky < kernel_size_; ++ky) {
                int new_y = std::clamp(static_cast<int>(y) + (ky - center), 0,
                                       static_cast<int>(picture_info.bmi_header.biHeight) - 1);
                new_blue += picture_info.pixels[new_y][x].blue * kernel_[ky];
                new_green += picture_info.pixels[new_y][x].green * kernel_[ky];
                new_red += picture_info.pixels[new_y][x].red * kernel_[ky];
            }
            image_copy[y][x].blue = static_cast<BYTE>(new_blue);
            image_copy[y][x].green = static_cast<BYTE>(new_green);
            image_copy[y][x].red = static_cast<BYTE>(new_red);
        }
    }
    for (LONG y = 0; y < picture_info.bmi_header.biHeight; ++y) {
        for (LONG x = 0; x < picture_info.bmi_header.biWidth; ++x) {
            new_blue = 0.0;
            new_green = 0.0;
            new_red = 0.0;
            for (int ky = 0; ky < kernel_size_; ++ky) {
                int new_x = std::clamp(static_cast<int>(x) + (ky - center), 0,
                                       static_cast<int>(picture_info.bmi_header.biWidth) - 1);
                new_blue += image_copy[y][new_x].blue * kernel_[ky];
                new_green += image_copy[y][new_x].green * kernel_[ky];
                new_red += image_copy[y][new_x].red * kernel_[ky];
            }

            picture_info.pixels[y][x].blue =
                static_cast<BYTE>(std::clamp(static_cast<LONG>(new_blue), 0, MaxColorValint));
            picture_info.pixels[y][x].green =
                static_cast<BYTE>(std::clamp(static_cast<LONG>(new_green), 0, MaxColorValint));
            picture_info.pixels[y][x].red =
                static_cast<BYTE>(std::clamp(static_cast<LONG>(new_red), 0, MaxColorValint));
        }
    }
}

void PixelizeFilter::Apply(PictureInfo &picture_info) {
    if (block_size_ <= 0) {
        throw InputDataException("Block size must be positive");
    }

    for (int y = 0; y < picture_info.bmi_header.biHeight; y += block_size_) {
        for (int x = 0; x < picture_info.bmi_header.biWidth; x += block_size_) {
            double avg_red = 0;
            double avg_green = 0;
            double avg_blue = 0;
            int counter = 0;

            for (int dy = 0; dy < block_size_ && y + dy < picture_info.bmi_header.biHeight; ++dy) {
                for (int dx = 0; dx < block_size_ && x + dx < picture_info.bmi_header.biWidth; ++dx) {
                    avg_red += picture_info.pixels[y + dy][x + dx].red;
                    avg_green += picture_info.pixels[y + dy][x + dx].green;
                    avg_blue += picture_info.pixels[y + dy][x + dx].blue;
                    ++counter;
                }
            }

            for (int dy = 0; dy < block_size_ && y + dy < picture_info.bmi_header.biHeight; ++dy) {
                for (int dx = 0; dx < block_size_ && x + dx < picture_info.bmi_header.biWidth; ++dx) {
                    picture_info.pixels[y + dy][x + dx].red = static_cast<BYTE>(avg_red / counter);
                    picture_info.pixels[y + dy][x + dx].green = static_cast<BYTE>(avg_green / counter);
                    picture_info.pixels[y + dy][x + dx].blue = static_cast<BYTE>(avg_blue / counter);
                }
            }
        }
    }
}
