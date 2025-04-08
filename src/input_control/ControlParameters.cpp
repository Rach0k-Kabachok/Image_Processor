#include <iostream>
#include <optional>
#include <memory>

#include "Filters.h"
#include "input_control/ControlParameters.h"
#include "Exceptions.h"

ControlParameters::ControlParameters(int argc, const char **argv) {
    for (int i = 0; i < argc; i++) {
        argv_.push_back(static_cast<std::string>(argv[i]));
    }
}

void ControlParameters::Control() {
    if (argv_.size() < 3) {
        std::cerr << "InputDataError: Too few arguments" << std::endl;
        return;
    }

    std::optional<PictureInfo> picture_info_opt;

    try {
        picture_info_opt = InputOutputProcessing::LoadBmpFile(argv_[1]);
    } catch (InputDataException &e) {
        std::cerr << "InputDataError: " << e.what() << std::endl;
        return;
    } catch (FileHeaderException &e) {
        std::cerr << "FileHeaderError: " << e.what() << std::endl;
        return;
    } catch (InfoHeaderException &e) {
        std::cerr << "InfoHeaderError: " << e.what() << std::endl;
        return;
    }

    if (!picture_info_opt) {
        return;
    }

    PictureInfo picture_info = *picture_info_opt;
    std::vector<std::unique_ptr<Filter>> filters;

    for (size_t ind = 3; ind < argv_.size(); ++ind) {
        std::string filter = argv_[ind];

        if (filter == "-gs") {
            filters.push_back(std::make_unique<GrayScaleFilter>());
        } else if (filter == "-neg") {
            filters.push_back(std::make_unique<NegativeFilter>());
        } else if (filter == "-sharp") {
            filters.push_back(std::make_unique<SharpeningFilter>());
        } else if (filter == "-edge") {
            if (ind + 1 >= argv_.size()) {
                std::cerr << "InputDataError: " << argv_[ind] << ": Missing value for" << argv_[ind] << std::endl;
            } else {
                std::string arg = argv_[ind + 1];
                try {
                    double threshold = std::stod(arg);
                    filters.push_back(std::make_unique<EdgeDetectionFilter>(threshold));
                    ind++;
                } catch (std::invalid_argument &) {
                    std::cerr << "InputDataError: " << argv_[ind] << ": Invalid type of argument" << std::endl;
                    return;
                }
            }
        } else if (filter == "-blur") {
            if (ind + 1 >= argv_.size()) {
                std::cerr << "InputDataError: " << ": Missing value for" << argv_[ind] << std::endl;
            } else {
                std::string arg = argv_[ind + 1];
                try {
                    double sigma = std::stod(arg);
                    filters.push_back(std::make_unique<GaussianBlurFilter>(sigma));
                    ind++;
                } catch (std::invalid_argument &) {
                    std::cerr << "InputDataError: " << argv_[ind] << ": Invalid type of argument" << std::endl;
                    return;
                }
            }
        } else if (filter == "-crop") {
            if (ind + 1 >= argv_.size()) {
                std::cerr << "InputDataError: " << ": Missing value for" << argv_[ind] << std::endl;
            } else {
                std::string arg1 = argv_[ind + 1];
                std::string arg2 = argv_[ind + 2];
                try {
                    LONG x_crop = std::stoi(arg1);
                    LONG y_crop = std::stoi(arg2);
                    filters.push_back(std::make_unique<CropFilter>(x_crop, y_crop));
                    ind += 2;
                } catch (std::invalid_argument &) {
                    std::cerr << "InputDataError: " << argv_[ind] << ": Invalid type of argument" << std::endl;
                    return;
                }
            }
        } else if (filter == "-pix") {
            if (ind + 1 >= argv_.size()) {
                std::cerr << "InputDataError: " << ": Missing value for" << argv_[ind] << std::endl;
            } else {
                std::string arg = argv_[ind + 1];
                try {
                    int block = std::stoi(arg);
                    filters.push_back(std::make_unique<PixelizeFilter>(block));
                    ind++;
                } catch (std::invalid_argument &) {
                    std::cerr << "InputDataError: " << argv_[ind] << ": Invalid type of argument" << std::endl;
                    return;
                }
            }
        } else {
            std::cerr << "Error: " << argv_[ind] << ": Unknown filter" << std::endl;
            return;
        }
    }

    for (const auto &filter : filters) {
        try {
            filter->Apply(picture_info);
        } catch (InputDataException &e) {
            std::cerr << "InputDataError: " << e.what() << std::endl;
            return;
        }
    }
    picture_info.Sync();
    InputOutputProcessing::SaveBmpFile(argv_[2], picture_info);
}
