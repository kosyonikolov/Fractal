#ifndef GENERATE_IMAGE_H
#define GENERATE_IMAGE_H

#include <stdint.h>
#include "Image.h"

void generateImage(Image * image,
				   const double offsetCompX, const double offsetCompY,
				   const double xScale, const double yScale,
				   const uint32_t maxIters);

#endif