#ifndef BMP_H
#define BMP_H
#include <stdint.h>
#include <string>

int saveBmpRgb(const std::string & filename, const uint8_t* image, const int width, const int height);

#endif