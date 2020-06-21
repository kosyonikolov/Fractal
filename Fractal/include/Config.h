#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <string>

#include "Verbosity.h"

enum class WorkAllocationType
{
	Dynamic,
	StaticOrdered
};

struct Config
{
    // size of image
	uint32_t width = 640;
	uint32_t height = 480;

	// size of rectangle in complex plane corresponding to image
	double compWidth = 2.0;
	double compHeight = 2.0;
	double compOffX = -1.0;
	double compOffY = -1.0;

	uint32_t threadCount = 1;
    uint32_t chunkWidth = 256;
	uint32_t chunkHeight = 64;
	uint32_t granularity = 0; // if > 0, overrides chunk width and height

	uint32_t minChunkWidth = 256;

	uint32_t maxIterations = 100;

    Verbosity verbosity = Verbosity::Stats;
	WorkAllocationType workAlloc = WorkAllocationType::Dynamic;

	std::string outputFileName = "zad19.bmp";

	std::string chunksFile = "chunks.txt";
};

bool parseConfig(char** args, const uint32_t count, Config * outConfig);

#endif