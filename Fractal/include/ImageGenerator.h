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
#include "Config.h"

class ImageGenerator
{
private:
    std::queue<ImageChunk> chunks;
    std::mutex chunkQueueMutex;

    std::vector<Worker*> workers;

    const Config & config;
    Image * const image;

    uint32_t allocCallCount = 0;

    // split the input image into chunks and push them to the queue
    void chunkify();

    bool allocateWork(Worker * worker);

    void reportStats();

public:
    ImageGenerator(Image * outputImage, const Config & config);

    ~ImageGenerator();

    Image * origImage;

    uint64_t run();
};

#endif