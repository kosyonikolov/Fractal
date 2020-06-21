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

	if (config.granularity > 0)
	{
		// override chunk size
		const uint32_t MIN_CHUNK_X = 256;
		const uint32_t xChunks = std::min((IMAGE_WIDTH / MIN_CHUNK_X) - (IMAGE_WIDTH % MIN_CHUNK_X > 0 ? 1 : 0), config.granularity);
		const uint32_t yChunks = config.granularity * config.threadCount / xChunks;

		std::cout << "Splitting into " << xChunks << "x" << yChunks << " chunks\n";
		config.chunkWidth = IMAGE_WIDTH / xChunks;
		config.chunkHeight = IMAGE_HEIGHT / yChunks;
	}

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

	//std::cout << "Generate image...\n";

	ImageGenerator generator(&outputImage, config);

	auto timeMs = generator.run();

	std::cout << "Generating took " << timeMs << " ms\n";

	//std::cout << "Save image...\n";
	saveBmpRgb(config.outputFileName, &outputImage);

	//std::cout << "Cleanup memory...\n";
	delete[] outputImage.data;
	return 0;
}