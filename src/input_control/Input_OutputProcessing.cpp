#include <vector>
#include "Exceptions.h"
#include "input_control/Input_OutputProcessing.h"

PictureInfo InputOutputProcessing::LoadBmpFile(const std::string &file_path) {
    std::ifstream infile(file_path, std::ios::binary);
    if (!infile.is_open()) {
        throw InputDataException("Wrong file path");
    }
    BmpFileHeader header{};
    BmpInfoHeader info_header{};

    infile.read(reinterpret_cast<char *>(&header), sizeof(BmpFileHeader));
    if (header.bfType != BM) {
        throw FileHeaderException("Incorrect file format");
    }

    infile.read(reinterpret_cast<char *>(&info_header), sizeof(BmpInfoHeader));
    if (info_header.biHeight == 0 || info_header.biWidth == 0) {
        throw FileHeaderException("Incorrect file size");
    }

    int padding = (4 - (info_header.biWidth) % 4) % 4;

    std::vector<std::vector<Pixel>> pixels(info_header.biHeight, std::vector<Pixel>(info_header.biWidth));

    for (int y = 0; y < info_header.biHeight; ++y) {
        for (int x = 0; x < info_header.biWidth; ++x) {
            Pixel pixel;
            if (!infile.read(reinterpret_cast<char *>(&pixel), sizeof(Pixel))) {
                throw std::runtime_error("Unexpected end of file");
            }
            pixels[y][x] = pixel;
        }
        infile.seekg(padding, std::ios::cur);
    }

    PictureInfo picture_info(header, info_header, pixels);
    infile.close();

    return picture_info;
}

void InputOutputProcessing::SaveBmpFile(const std::string &file_path, PictureInfo &picture_info) {
    std::ofstream outfile(file_path, std::ios::binary);

    int padding = (-(picture_info.bmi_header.biWidth * 3)) % 4;
    if (padding < 0) {
        padding += 4;
    }
    outfile.write(reinterpret_cast<char *>(&picture_info.bmf_header), sizeof(BmpFileHeader));
    outfile.write(reinterpret_cast<char *>(&picture_info.bmi_header), sizeof(BmpInfoHeader));

    for (std::vector<Pixel> &row : picture_info.pixels) {
        for (Pixel &pixel : row) {
            outfile.write(reinterpret_cast<char *>(&pixel), sizeof(Pixel));
        }
        for (int i = 0; i < padding; ++i) {
            outfile.put(0);
        }
    }

    outfile.close();
}
