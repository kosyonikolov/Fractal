#include <complex>
#include "generateImage.h"

// Escape time pixel value algorithm
// There is no escaper radius for this formula, so just loop until
// the values become inf or nan
static inline uint32_t getExitIters(const double x, const double y, const uint32_t maxIters, double * outSum, double * outTheta)
{
	const std::complex<double> c(x, y);
	std::complex<double> z(0, 0);

	int i = 0;
	double sum = 0;
	std::complex<double> lastZ = z;

	for (; i < maxIters; i++)
	{
		//auto sq = z * z;
		//z = std::exp(sq) * sq + c;
		z = std::exp(z * z + c);
		if (!std::isfinite(z.real()) || !std::isfinite(z.imag()))
		{
			break;
		}

		lastZ = z;

		const double r = std::abs(z);
		if (std::isfinite(r))
		{
			sum += std::exp(-r);
		}
	}

	*outTheta = std::atan2(lastZ.real(), lastZ.imag());;
	*outSum = sum + std::log(double(i));
	return i;
}

void generateImage(Image * image, FloatImage * dbgImage,
				   const double offsetCompX, const double offsetCompY,
				   const double xScale, const double yScale,
				   const uint32_t maxIters, const RgbLut * lut)
{
	uint8_t* imgLine = image->data;
	double * dbgLine = dbgImage->data;
	for (uint32_t y = 0; y < image->height; y++)
	{
		for (uint32_t x = 0; x < image->width; x++)
		{
			const double compX = x * xScale + offsetCompX;
			const double compY = y * yScale + offsetCompY;

			double sum = 0;
			double theta = 0;
			const uint32_t iters = getExitIters(compX, compY, maxIters, &sum, &theta);

			imgLine[3 * x] = lut->r[iters];;
			imgLine[3 * x + 1] = lut->g[iters];
			imgLine[3 * x + 2] = lut->b[iters];
			dbgLine[2 * x] = sum;
			dbgLine[2 * x + 1] = theta;
		}

		imgLine += image->stride;
		dbgLine += dbgImage->stride;
	}
}