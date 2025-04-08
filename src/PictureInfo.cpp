#include <cmath>
#include "PictureInfo.h"

Pixel PictureInfo::CheckingBorders(LONG x, LONG y, PictureInfo &picture_info) {
    if (x < 0) {
        if (y < 0) {
            return picture_info.pixels[y + 1][x + 1];
        }
        if (y >= picture_info.bmi_header.biHeight) {
            return picture_info.pixels[y - 1][x + 1];
        }
        return picture_info.pixels[y][x + 1];
    }
    if (x >= picture_info.bmi_header.biWidth) {
        if (y < 0) {
            return picture_info.pixels[y + 1][x - 1];
        }
        if (y >= picture_info.bmi_header.biHeight) {
            return picture_info.pixels[y - 1][x - 1];
        }
        return picture_info.pixels[y][x - 1];
    }
    if (y < 0) {
        return picture_info.pixels[y + 1][x];
    }
    if (y >= picture_info.bmi_header.biHeight) {
        return picture_info.pixels[y - 1][x];
    }
    return picture_info.pixels[y][x];
}

void PictureInfo::Sync() {
    if (pixels.empty() || pixels[0].empty()) {
        bmi_header.biHeight = 0;
        bmi_header.biWidth = 0;
        bmi_header.biSizeImage = 0;
        bmi_header.biSize = DefaultBisize;
        bmf_header.bfSize = DefaultBfsize;
    } else {
        int padding = (4 - (bmi_header.biWidth * 3) % 4) % 4;

        bmi_header.biSizeImage = (bmi_header.biWidth * 3 + padding) * abs(bmi_header.biHeight);
        bmi_header.biHeight = static_cast<LONG>(pixels.size());
        bmi_header.biWidth = static_cast<LONG>(pixels[0].size());
        bmi_header.biSize = DefaultBisize;
        bmf_header.bfSize = DefaultBfsize + bmi_header.biSizeImage;
    }
}
