#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

#include "bmp.h"
#include "RgbLut.h"
#include "generateImage.h"
#include "ImageGenerator.h"

using namespace std;

int main(int argc, char ** argv)
{
	// image dimensions
	const int IMAGE_WIDTH = 1920;
	const int IMAGE_HEIGHT = 1080;

	// complex plane
	const double PLANE_X_START = -1.0;
	const double PLANE_X_END = 1.0;
	const double PLANE_Y_START = -1.0;
	const double PLANE_Y_END = 1.0;

	const double WIDTH_2_PLANE = (PLANE_X_END - PLANE_X_START) / (IMAGE_WIDTH - 1);
	const double HEIGHT_2_PLANE = (PLANE_Y_END - PLANE_Y_START) / (IMAGE_HEIGHT - 1);

	const int MAX_ITERS = 100;

	Image outputImage;
	outputImage.data = new uint8_t[3 * IMAGE_HEIGHT * IMAGE_WIDTH];
	outputImage.width = IMAGE_WIDTH;
	outputImage.height = IMAGE_HEIGHT;
	outputImage.stride = 3 * IMAGE_WIDTH;

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

	int availableThreads = std::thread::hardware_concurrency();
	std::cout << "Hardware threads: " << availableThreads << std::endl;

	auto start = std::chrono::steady_clock::now();

	ImageGenerator generator(&outputImage, 
							 PLANE_X_START, PLANE_Y_START, 
							 WIDTH_2_PLANE, HEIGHT_2_PLANE, 
							 MAX_ITERS, &lut,
							 4, 4);

	generator.run();

	auto end = std::chrono::steady_clock::now();

	uint64_t timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Generating took " << timeMs << " ms\n";

	std::cout << "Save image...\n";
	saveBmpRgb("fractal2.bmp", &outputImage);

	std::cout << "Cleanup memory...\n";
	delete[] outputImage.data;
	return 0;
}