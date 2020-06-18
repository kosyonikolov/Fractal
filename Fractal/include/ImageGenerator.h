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

struct ImageChunk
{
    Image image;
    double offsetX, offsetY;
    double scaleX, scaleY;
};

class Worker
{
public:
    struct Stats
    {
        uint64_t time; // total runtime, milliseconds
        uint32_t chunkCount; // number of chunks processed by the worker
    };

    // Function that allocates work to the argument worker
    // Return value indicates success: on true, work was succesfully allocated
    // false means that no work was allocated and more work will not be available
    typedef std::function<bool(Worker*)> WorkAllocatorFunction;

    void run();

    void addChunk(const ImageChunk & chunk);

    Stats getExitStats() { return exitStats; }

    // Initialize a worker
    // allocator can be null - in this case the worker will stop
    // when the work chunks are over
    Worker(const uint32_t maxIters, WorkAllocatorFunction allocator) : maxPixelIterations(maxIters), allocateWork(allocator) {};
    
private:
    // chunks that are waiting to be processed
    std::queue<ImageChunk> chunks;
        
    WorkAllocatorFunction allocateWork = 0;

    const uint32_t maxPixelIterations;

    Stats exitStats;
};

class ImageGenerator
{
private:
    std::queue<ImageChunk> chunks;
    std::mutex chunkQueueMutex;

    std::vector<Worker*> workers;

    const uint32_t threadCount = 1; 

    const uint32_t maxIters = 100;

    // split the input image into chunks and push them to the queue
    void chunkify(const Image * image,
                  const double offsetX, const double offsetY,
                  const double scaleX, const double scaleY,
                  const uint32_t count);

    bool allocateWork(Worker * worker);

public:
    ImageGenerator(Image * outputImage,
                   const double offsetX, const double offsetY,
                   const double scaleX, const double scaleY,
                   const uint32_t maxIters,
                   const uint32_t threadCount, const uint32_t granularity);

    Image * origImage;

    void run();
};

#endif