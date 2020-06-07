#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "bmp.h"
#include "RgbLut.h"

using namespace std;

const int MAX_ITERS = 50;

inline void calcComplexSquare(double& real, double& imaginary)
{
	const double newReal = real * real - imaginary * imaginary;
	const double newImaginary = 2.0 * real * imaginary;

	real = newReal;
	imaginary = newImaginary;
}

inline double iterNormal(double& real, double& imaginary, const double x, const double y)
{
	calcComplexSquare(real, imaginary);
	real += x;
	imaginary += y;
	
	return real * real + imaginary * imaginary;
}

inline uint32_t pixelValue(const double x, const double y)
{
	const double MAX_RAD = 2.0;

	double distance = 0;
	int i = 0;
	double real = 0;
	double imaginary = 0;
	for (; i < MAX_ITERS; i++)
	{
		distance = iterNormal(real, imaginary, x, y);
		if (!std::isfinite(distance))
		{
			return i;
		}
	}
	return 0;
}

void generateImage(uint8_t* image,
	const uint32_t width, const uint32_t height, const uint32_t stride,
	const double offsetCompX, const double offsetCompY,
	const double xScale, const double yScale,
	const RgbLut & lut)
{
	uint8_t* imgLine = image;
	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			const double compX = x * xScale + offsetCompX;
			const double compY = y * yScale + offsetCompY;

			const uint32_t value = pixelValue(compX, compY);
			imgLine[3 * x] = lut.r[value];
			imgLine[3 * x + 1] = lut.g[value];
			imgLine[3 * x + 2] = lut.b[value];
		}

		imgLine += stride;
	}
}

int main(int argc, char ** argv)
{
	// image dimensions
	const int IMAGE_WIDTH = 1920;
	const int IMAGE_HEIGHT = 1080;

	// complex plane
	const double PLANE_X_START = -2.0;
	const double PLANE_X_END = 2.0;
	const double PLANE_Y_START = -2.0;
	const double PLANE_Y_END = 2.0;

	const double WIDTH_2_PLANE = (PLANE_X_END - PLANE_X_START) / (IMAGE_WIDTH - 1);
	const double HEIGHT_2_PLANE = (PLANE_Y_END - PLANE_Y_START) / (IMAGE_HEIGHT - 1);

	cv::Mat outputImage(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3);

	// color LUT generation
	RgbLut lut;
	{
		lut.size = MAX_ITERS;

		std::vector<ColorStop> stops;
		stops.push_back({ 0.0, 1, 1, 1 });
		stops.push_back({ 0.5, 128, 0, 128 });
		stops.push_back({ 1.0, 0, 0, 255 });

		createLut(stops, lut);
	}

	for (int y = 0; y < IMAGE_HEIGHT; y++)
	{
		uint8_t* outLine = outputImage.ptr<uint8_t>(y);
		for (int x = 0; x < IMAGE_WIDTH; x++)
		{
			const double cx = x * WIDTH_2_PLANE + PLANE_X_START;
			const double cy = y * HEIGHT_2_PLANE + PLANE_Y_START;
			
			uint8_t val = pixelValue(cx, cy);
			outLine[3 * x] = lut.r[val];
			outLine[3 * x + 1] = lut.g[val];
			outLine[3 * x + 2] = lut.b[val];
		}
	}

	cv::imwrite("fractal.bmp", outputImage);

	saveBmpRgb("fractal2.bmp", outputImage.data, IMAGE_WIDTH, IMAGE_HEIGHT);
	return 0;
}