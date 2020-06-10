#ifndef COMMAND_ARGS_PARSER_H
#define COMMAND_ARGS_PARSER_H

#include <stdint.h>
#include <string>

class CommandArgsParser
{
private:
	// size of image
	uint32_t width = 640;
	uint32_t height = 480;

	// size of rectangle in complex plane corresponding to image
	double compWidth = 2.0;
	double compHeight = 2.0;
	double compOffX = -1.0;
	double compOffY = -1.0;

	uint32_t threadCount = 1;

	std::string outputFileName = "zad19.bmp";

public:
	void Parse(char** args, const uint32_t count);

	void GetImageSize(uint32_t& outWidth, uint32_t outHeight);
	void GetComplexPlaneSize(double& outWidth, double& outHeight);
	void GetComplexPlaneOffset(double& outOffX, double& outOffY);
	std::string GetOutputFileName();
	uint32_t GetThreadCount();
};

#endif