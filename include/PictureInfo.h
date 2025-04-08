#ifndef PICTURE_INFO_H
#define PICTURE_INFO_H

#include <vector>
#include <cstdint>

using WORD = uint16_t;
using DWORD = uint32_t;
using LONG = int32_t;
using BYTE = unsigned char;

constexpr DWORD DefaultBisize = 40;
constexpr DWORD DefaultBfsize = 14;

struct Pixel {
    BYTE blue;
    BYTE green;
    BYTE red;
};

#pragma pack(push, 1)
using BmpFileHeader = struct BmpFileHeader {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};
#pragma pack(pop)

#pragma pack(push, 1)
using BmpInfoHeader = struct BmpInfoHeader {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};
#pragma pack(pop)

struct PictureInfo {
    BmpFileHeader bmf_header;
    BmpInfoHeader bmi_header;
    std::vector<std::vector<Pixel> > pixels;

    PictureInfo(BmpFileHeader &bmf_header, BmpInfoHeader &bmi_header, std::vector<std::vector<Pixel> > &pixels)
        : bmf_header(bmf_header), bmi_header(bmi_header), pixels(pixels) {
    }

    Pixel CheckingBorders(LONG x, LONG y, PictureInfo &picture_info);

    void Sync();
};

#endif  // PICTURE_INFO_H
