#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

struct Image
{
    uint8_t * data;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
};

struct FloatImage
{
    double * data;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
};

#endif