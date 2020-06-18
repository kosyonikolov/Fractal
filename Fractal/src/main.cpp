#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

#include "bmp.h"
#include "ImageGenerator.h"
#include "Config.h"

using namespace std;

int main(int argc, char ** argv)
{
	Config config;
	if (!parseConfig(argv + 1, argc - 1, &config))
	{
		std::cerr << "Bad command line arguments. Bye.\n";
		return 1;
	}

	// image dimensions
	const int IMAGE_WIDTH = config.width;
	const int IMAGE_HEIGHT = config.height;

	// scales
	const double X_SCALE = config.compWidth / IMAGE_WIDTH;
	const double Y_SCALE = config.compHeight / IMAGE_HEIGHT;

	const int MAX_ITERS = 100;

	// create image
	Image outputImage;
	outputImage.data = new uint8_t[3 * IMAGE_HEIGHT * IMAGE_WIDTH];
	outputImage.width = IMAGE_WIDTH;
	outputImage.height = IMAGE_HEIGHT;
	outputImage.stride = 3 * IMAGE_WIDTH;

	std::cout << "Generate image...\n";

	int availableThreads = std::thread::hardware_concurrency();
	std::cout << "Hardware threads: " << availableThreads << std::endl;

	auto start = std::chrono::steady_clock::now();

	ImageGenerator generator(&outputImage,
							 config.compOffX, config.compOffY, 
							 X_SCALE, Y_SCALE, 
							 MAX_ITERS,
							 config.threadCount, config.granularity);

	generator.run();

	auto end = std::chrono::steady_clock::now();

	uint64_t timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Generating took " << timeMs << " ms\n";

	std::cout << "Save image...\n";
	saveBmpRgb(config.outputFileName, &outputImage);

	std::cout << "Cleanup memory...\n";
	delete[] outputImage.data;
	return 0;
}