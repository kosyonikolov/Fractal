#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

int main(int argc, char ** argv)
{
	const int WIDTH = 640;
	const int HEIGHT = 480;
	cv::Mat image(HEIGHT, WIDTH, CV_8UC1);

	const int CENTER_X = WIDTH / 2;
	const int CENTER_Y = HEIGHT / 2;

	for (int y = 0; y < image.rows; y++)
	{
		uint8_t* outLine = image.ptr<uint8_t>(y);
		for (int x = 0; x < image.cols; x++)
		{
			const double dx = x - CENTER_X;
			const double dy = y - CENTER_Y;
			const double dist = std::sqrt(dx * dx + dy * dy);

			const double value = std::max(0.0, std::min(255.0, (255.0 - dist)));
			outLine[x] = uint8_t(value);
		}
	}

	cv::imwrite("circle.bmp", image);
	return 0;
}