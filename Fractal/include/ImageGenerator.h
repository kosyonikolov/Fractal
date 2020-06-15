#ifndef IMAGE_GENERATOR_H
#define IMAGE_GENERATOR_H

#include <stdint.h>
#include <thread>
#include <functional>
#include <queue>
#include <chrono>

#include "Image.h"
#include "RgbLut.h"

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

    Stats run();

    void addChunk(const ImageChunk & chunk);

    // Initialize a worker
    // allocator can be null - in this case the worker will stop
    // when the work chunks are over
    Worker(const uint32_t maxIters, const RgbLut * lut,
           WorkAllocatorFunction allocator) : maxPixelIterations(maxIters), lut(lut), allocateWork(allocator) {};
 
private:
    // chunks that are waiting to be processed
    std::queue<ImageChunk> chunks;
    
    WorkAllocatorFunction allocateWork = 0;

    const uint32_t maxPixelIterations;
    const RgbLut * lut;
};

class ImageGenerator
{
private:
    std::queue<ImageChunk> chunks;

    const uint32_t threadCount = 1; 

    void chunkify();

public:


    void run();
};

#endif