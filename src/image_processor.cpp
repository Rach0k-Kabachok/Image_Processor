#include "../include/input_control/ControlParameters.h"

int main(int argc, const char* argv[]) {
    ControlParameters bmp(argc, argv);
    bmp.Control();
    return 0;
}
