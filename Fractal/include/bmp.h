#ifndef BMP_H
#define BMP_H
#include <stdint.h>
#include <string>
#include "Image.h"

int saveBmpRgb(const std::string & filename, const Image * image);

#endif