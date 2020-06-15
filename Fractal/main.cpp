#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <complex>
#include <chrono>

#include "bmp.h"
#include "RgbLut.h"
#include "generateImage.h"

using namespace std;

int main(int argc, char ** argv)
{
	// image dimensions
	const int IMAGE_WIDTH = 640;
	const int IMAGE_HEIGHT = 480;

	// complex plane
	const double PLANE_X_START = -1.0;
	const double PLANE_X_END = 1.0;
	const double PLANE_Y_START = -1.0;
	const double PLANE_Y_END = 1.0;

	const double WIDTH_2_PLANE = (PLANE_X_END - PLANE_X_START) / (IMAGE_WIDTH - 1);
	const double HEIGHT_2_PLANE = (PLANE_Y_END - PLANE_Y_START) / (IMAGE_HEIGHT - 1);

	const int MAX_ITERS = 100;

	uint8_t* outputImage = new uint8_t[3 * IMAGE_HEIGHT * IMAGE_WIDTH];

	// color LUT generation
	std::cout << "Create LUT...\n";
	RgbLut lut;
	{
		lut.size = MAX_ITERS + 1;

		std::vector<ColorStop> stops;
		stops.push_back({ 0.0, 255, 0, 0 });
		stops.push_back({ 0.1, 0, 0, 255 });

		createLut(stops, lut);
	}

	std::cout << "Generate image...\n";

	auto start = std::chrono::steady_clock::now();

	generateImage(outputImage,
		IMAGE_WIDTH, IMAGE_HEIGHT, 3 * IMAGE_WIDTH,
		PLANE_X_START, PLANE_Y_START, WIDTH_2_PLANE, HEIGHT_2_PLANE,
		MAX_ITERS, lut);

	auto end = std::chrono::steady_clock::now();

	uint64_t timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Generating took " << timeMs << " ms\n";

	std::cout << "Save image...\n";
	saveBmpRgb("fractal2.bmp", outputImage, IMAGE_WIDTH, IMAGE_HEIGHT);

	std::cout << "Cleanup memory...\n";
	delete[] outputImage;
	return 0;
}