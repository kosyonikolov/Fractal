#include <complex>
#include "generateImage.h"

// Escape time pixel value algorithm
// There is no escape radius for this formula, so just loop until
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

	// calculate angle just once
	*outTheta = std::arg(lastZ);
	*outSum = sum;
	return i;
}

void generateImage(Image * image,
				   const double offsetCompX, const double offsetCompY,
				   const double xScale, const double yScale,
				   const uint32_t maxIters)
{
	for (uint32_t y = 0; y < image->height; y++)
	{
		uint8_t* imgLine = image->data + y * image->stride;
		for (uint32_t x = 0; x < image->width; x++)
		{
			const double compX = x * xScale + offsetCompX;
			const double compY = y * yScale + offsetCompY;

			double sum = 0;
			double theta = 0;
			getExitIters(compX, compY, maxIters, &sum, &theta);
			theta += 3.14 / 2.0;

			// create color in YUV domain
			const double y = 180.0 * 0.5 * (std::cos(0.8 * sum) + 1);
			const double u = 200.0 * 0.5 * (std::sin(theta) + 1);
			const double v = 200.0 * 0.5 * (std::cos(theta) + 1);

			// convert to rgb on the fly
			*imgLine++ = std::min(255.0, std::max(0.0, y + 1.772 * (v - 128)));
			*imgLine++ = std::min(255.0, std::max(0.0, y - 0.344136 * (v - 128) - 0.714136 * (u - 128)));
			*imgLine++ = std::min(255.0, std::max(0.0, y + 1.402 * (u - 128)));
		}
	}
}