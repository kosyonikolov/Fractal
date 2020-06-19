#ifndef IMAGE_GENERATOR_H
#define IMAGE_GENERATOR_H

#include <stdint.h>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <chrono>
#include <vector>
#include <iostream>

#include "Image.h"
#include "Verbosity.h"
#include "Worker.h"

class ImageGenerator
{
private:
    std::queue<ImageChunk> chunks;
    std::mutex chunkQueueMutex;

    std::vector<Worker*> workers;

    const uint32_t threadCount = 1; 

    const uint32_t maxIters = 100;

    const Verbosity v = Verbosity::Silent;

    uint32_t allocCallCount = 0;

    // split the input image into chunks and push them to the queue
    void chunkify(const Image * image,
                  const double offsetX, const double offsetY,
                  const double scaleX, const double scaleY,
                  const uint32_t width, const uint32_t height);

    bool allocateWork(Worker * worker);

    void reportStats();

public:
    ImageGenerator(Image * outputImage,
                   const double offsetX, const double offsetY,
                   const double scaleX, const double scaleY,
                   const uint32_t maxIters,
                   const uint32_t threadCount,
                   const uint32_t width, const uint32_t height,
                   const Verbosity v);

    ~ImageGenerator();

    Image * origImage;

    void run();
};

#endif