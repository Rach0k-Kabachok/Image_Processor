#ifndef INPUT_PROCESSING_H
#define INPUT_PROCESSING_H

#include <fstream>
#include "PictureInfo.h"

constexpr WORD BM = 19778;

struct InputOutputProcessing {
    static PictureInfo LoadBmpFile(const std::string &file_path);

    static void SaveBmpFile(const std::string &file_path, PictureInfo &picture_info);
};
#endif  // INPUT_PROCESSING_H
