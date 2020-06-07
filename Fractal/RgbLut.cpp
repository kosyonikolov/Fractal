#include "RgbLut.h"

void createLut(const std::vector<ColorStop> colorStops, RgbLut& outLut)
{
	const int LUT_SIZE = outLut.size;

	// allocate memory
	outLut.r = new uint8_t[LUT_SIZE];
	outLut.g = new uint8_t[LUT_SIZE];
	outLut.b = new uint8_t[LUT_SIZE];

	// set zero value to black -- coresponding to pt inside fractal set
	outLut.r[0] = 0;
	outLut.g[0] = 0;
	outLut.b[0] = 0;

	// ------------------------------
	// ---- linear interpolation ----
	// ------------------------------

	// last table idx corresponds to 1.0, idx 1 corresponds to 0.0
	const double IDX_TO_VAL = 1.0 / double(LUT_SIZE - 2);
	for (int i = 1; i < LUT_SIZE; i++)
	{
		const double value = (i - 1) * IDX_TO_VAL;

		// linear search: find between which stops the current value sits
		// j holds the idx of the first stop that has a higher value
		int j = 0;
		for (; j < colorStops.size(); j++)
		{
			if (colorStops[j].value > value) break;
		}

		if (j == 0)
		{
			// pixel is lower than all color stops -> assign lowest color
			const ColorStop& high = colorStops[j];
			outLut.r[i] = high.r;
			outLut.g[i] = high.g;
			outLut.b[i] = high.b;
		}
		else if (j == colorStops.size())
		{
			// pixel is higher than all colors -> assign highest color
			const ColorStop& low = colorStops[j - 1];
			outLut.r[i] = low.r;
			outLut.g[i] = low.g;
			outLut.b[i] = low.b;
		}
		else
		{
			// pixel is between two color stops - interpolate value
			const ColorStop& high = colorStops[j];
			const ColorStop& low = colorStops[j - 1];

			const double mixHigh = (value - low.value) / (high.value - low.value);
			const double mixLow = 1.0 - mixHigh;

			outLut.r[i] = mixHigh * high.r + mixLow * low.r;
			outLut.g[i] = mixHigh * high.g + mixLow * low.g;
			outLut.b[i] = mixHigh * high.b + mixLow * low.b;
		}
	}
}