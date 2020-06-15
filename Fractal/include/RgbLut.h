#ifndef RGB_LUT_H
#define RGB_LUT_H

#include <stdint.h>
#include <vector>

struct RgbLut
{
	uint32_t size;
	uint8_t* r;
	uint8_t* g;
	uint8_t* b;
};

struct ColorStop
{
	double value; // 0..1
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

// populate the RGB table by interpolating between the supplied color stops
// the color stops are expected to be sorted in an ascending order of values
// output LUT size is the input's outLut.size
// this function allocates the LUT memory with new
void createLut(const std::vector<ColorStop> colorStops, RgbLut & outLut);

#endif
