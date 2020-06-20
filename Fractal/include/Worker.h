#ifndef WORKER_H
#define WORKER_H

#include <stdint.h>
#include <queue>
#include <functional>
#include "Image.h"
#include "Verbosity.h"
#include "Config.h"

struct ImageChunk
{
    uint32_t id;
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
        uint64_t waitTime; // total time waiting for work, milliseconds
        uint32_t chunkCount; // number of chunks processed by the worker
    };

    struct ProcessedChunk : ImageChunk
    {
        uint64_t time; // millseconds
    };

    // Function that allocates work to the argument worker
    // Return value indicates success: on true, work was succesfully allocated
    // false means that no work was allocated and more work will not be available
    typedef std::function<bool(Worker*)> WorkAllocatorFunction;

    const uint32_t id;
    Stats exitStats;
    std::vector<ProcessedChunk> processedChunks;

    void run();

    void addChunk(const ImageChunk & chunk);

    // Initialize a worker
    // allocator can be null - in this case the worker will stop
    // when the work chunks are over
    Worker(const uint32_t id, WorkAllocatorFunction allocator, const Config & config) : 
                id(id), allocateWork(allocator), config(config) {};
    
private:
    // chunks that are waiting to be processed
    std::queue<ImageChunk> chunks;
    WorkAllocatorFunction allocateWork = 0;
    
    const Config & config;
};

#endif