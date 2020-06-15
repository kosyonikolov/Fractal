#ifndef GENERATE_IMAGE_H
#define GENERATE_IMAGE_H

#include <stdint.h>
#include "RgbLut.h"

void generateImage(uint8_t* image,
	const uint32_t width, const uint32_t height, const uint32_t stride,
	const double offsetCompX, const double offsetCompY,
	const double xScale, const double yScale,
	const uint32_t maxIters, const RgbLut& lut);

#endif