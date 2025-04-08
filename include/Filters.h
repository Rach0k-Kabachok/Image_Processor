#ifndef FILTERS_H
#define FILTERS_H

#include "PictureInfo.h"

constexpr int MaxColorValint = 255;
constexpr double MaxColorValdouble = 255.0;
constexpr double GrayRed = 0.299;
constexpr double GrayGreen = 0.587;
constexpr double GrayBlue = 0.114;
constexpr int SizeOfKernel = 6;
constexpr double PI = 3.1415;
constexpr double EXP = 2.7182;

struct Filter {

    Filter() = default;

    virtual void Apply(PictureInfo &picture_info) = 0;

    virtual ~Filter() = default;
};

struct NegativeFilter : public Filter {
    using Filter::Filter;

    void Apply(PictureInfo &picture_info) override;
};

struct GrayScaleFilter : public Filter {
    using Filter::Filter;

    void Apply(PictureInfo &picture_info) override;
};

class EdgeDetectionFilter : public GrayScaleFilter {
private:
    double threshold_;

public:
    explicit EdgeDetectionFilter(double threshold) : GrayScaleFilter(), threshold_(threshold) {
    }

    void Apply(PictureInfo &picture_info) override;
};

struct SharpeningFilter : public Filter {
    using Filter::Filter;

    void Apply(PictureInfo &picture_info) override;
};

class GaussianBlurFilter : public Filter {
private:
    double sigma_;
    LONG kernel_size_;
    std::vector<double> kernel_;

    void CreateGaussianKernel();

public:
    explicit GaussianBlurFilter(double sigma)
        : Filter(), sigma_(sigma), kernel_size_(SizeOfKernel * static_cast<int>(sigma_) + 1) {
    }

    void Apply(PictureInfo &picture_info) override;
};

class CropFilter : public Filter {
private:
    LONG x_crop_;
    LONG y_crop_;

public:
    CropFilter(LONG x_crop, LONG y_crop) : Filter(), x_crop_(x_crop), y_crop_(y_crop) {
    }

    void Apply(PictureInfo &picture_info) override;
};

class PixelizeFilter : public Filter {
private:
    int block_size_;

public:
    explicit PixelizeFilter(int block_size) : Filter(), block_size_(block_size) {
    }

    void Apply(PictureInfo &picture_info) override;
};

#endif  // FILTERS_H
