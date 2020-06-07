#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

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

inline uint8_t pixelValue(const double x, const double y)
{
	const int MAX_ITERS = 100;
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
			return (i + 1) * 255 / MAX_ITERS;
		}
	}
	return 0;
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

	cv::Mat outputImage(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);

	for (int y = 0; y < IMAGE_HEIGHT; y++)
	{
		uint8_t* outLine = outputImage.ptr<uint8_t>(y);
		for (int x = 0; x < IMAGE_WIDTH; x++)
		{
			const double cx = x * WIDTH_2_PLANE + PLANE_X_START;
			const double cy = y * HEIGHT_2_PLANE + PLANE_Y_START;
			
			uint8_t val = pixelValue(cx, cy);
			outLine[x] = val;
		}
	}

	cv::imwrite("fractal.bmp", outputImage);
	return 0;
}