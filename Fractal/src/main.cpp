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

	FloatImage dbgImage;
	dbgImage.data = new double[2 * IMAGE_WIDTH * IMAGE_HEIGHT];
	dbgImage.width = IMAGE_WIDTH;
	dbgImage.height = IMAGE_HEIGHT;
	dbgImage.stride = 2 * IMAGE_WIDTH;

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

	ImageGenerator generator(&outputImage, &dbgImage,
							 PLANE_X_START, PLANE_Y_START, 
							 WIDTH_2_PLANE, HEIGHT_2_PLANE, 
							 MAX_ITERS, &lut,
							 4, 16);

	generator.run();

	auto end = std::chrono::steady_clock::now();

	uint64_t timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Generating took " << timeMs << " ms\n";

	std::cout << "Save image...\n";
	saveBmpRgb("fractal2.bmp", &outputImage);

	// analyze debug image
	{
		std::cout << "Convert to 8 bit...\n";

		auto yuv2Rgb = [](const double y, const double u, const double v, double *r, double *g, double *b)
		{
			*r = std::min(255.0, std::max(0.0, y + 1.402 * (u - 128)));
			*g = std::min(255.0, std::max(0.0, y - 0.344136 * (v - 128) - 0.714136 * (u - 128)));
			*b = std::min(255.0, std::max(0.0, y + 1.772 * (v - 128)));
		};

		auto hsv2rgb = [](double h, double s, double v, double * r, double * g, double * b)
		{
			double      hh, p, q, t, ff;
			long        i;

			hh = h;
			if(hh >= 360.0)
			{
				hh -= 360;
			} 
			hh /= 60.0;
			i = (long)hh;
			ff = hh - i;
			p = v * (1.0 - s);
			q = v * (1.0 - (s * ff));
			t = v * (1.0 - (s * (1.0 - ff)));

			switch(i) 
			{
			case 0:
				*r = v;
				*g = t;
				*b = p;
				break;
			case 1:
				*r = q;
				*g = v;
				*b = p;
				break;
			case 2:
				*r = p;
				*g = v;
				*b = t;
				break;

			case 3:
				*r = p;
				*g = q;
				*b = v;
				break;
			case 4:
				*r = t;
				*g = p;
				*b = v;
				break;
			case 5:
			default:
				*r = v;
				*g = p;
				*b = q;
				break;
			}

			*r = std::max(0.0, std::min(255.0, *r));
			*g = std::max(0.0, std::min(255.0, *g));
			*b = std::max(0.0, std::min(255.0, *b));
		};

		const double PI = 3.14159265359;

		for (uint32_t y = 0; y < IMAGE_HEIGHT; y++)
		{
			uint8_t * imgLine = outputImage.data + y * outputImage.stride;
			double * dbgLine = dbgImage.data + y * dbgImage.stride;
			for (uint32_t x = 0; x < IMAGE_WIDTH; x++)
			{
				const double val = *dbgLine++;
				const double theta = *dbgLine++;

				double r, g, b;

				double y = 180.0 * 0.5 * (std::cos(0.8*val) + 1);
				double u = 200.0 * 0.5 * (std::sin(theta) + 1);
				double v = 200.0 * 0.5 * (std::cos(theta) + 1);
				yuv2Rgb(y, u, v, &r, &g, &b);

				//const double h = (180.0 + theta * 180.0 / PI) / 2.0 - 90;
				//const double s = 0.7;
				//const double v = 25 + 230 * 0.5 * (std::sin(val) + 1);
				//hsv2rgb(h, s, v, &r, &g, &b);

				*imgLine++ = b;
				*imgLine++ = g;
				*imgLine++ = r;
			}
		}	

		std::cout << "Save debug img...\n";
		saveBmpRgb("debug_image.bmp", &outputImage);
	}

	std::cout << "Cleanup memory...\n";
	delete[] outputImage.data;
	return 0;
}