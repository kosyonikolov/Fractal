#ifndef GENERATE_IMAGE_H
#define GENERATE_IMAGE_H

#include <stdint.h>
#include "RgbLut.h"
#include "Image.h"

void generateImage(Image * image, FloatImage * dbgImage,
				   const double offsetCompX, const double offsetCompY,
				   const double xScale, const double yScale,
				   const uint32_t maxIters, const RgbLut * lut);

#endif