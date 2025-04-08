#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <utility>

#include "Input_OutputProcessing.h"

class ControlParameters {
public:
    ControlParameters(int argc, const char **argv);

    explicit ControlParameters(std::vector<std::string> argv) : argv_(std::move(argv)) {
    }

    void Control();

private:
    std::vector<std::string> argv_;
};

#endif  // CONTROLLER_H
