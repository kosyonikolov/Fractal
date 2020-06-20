#include "ImageGenerator.h"
#include "generateImage.h"

#include <cmath>

void ImageGenerator::chunkify(const Image * image)
{
    uint32_t chunkId = 0;

    const uint32_t CHUNK_HEIGHT = this->config.chunkHeight;
    const uint32_t CHUNK_WIDTH = this->config.chunkWidth;

    const double OFFSET_X = this->config.compOffX;
    const double OFFSET_Y = this->config.compOffY;
    const double SCALE_X = this->config.compWidth / image->width;
    const double SCALE_Y = this->config.compHeight / image->height;

    for (uint32_t yStart = 0; yStart < image->height; yStart += CHUNK_HEIGHT)
    {
        const uint32_t chunkHeight = std::min(CHUNK_HEIGHT, image->height - yStart);
        for (uint32_t xStart = 0; xStart < image->width; xStart += CHUNK_WIDTH)
        {
            const uint32_t chunkWidth = std::min(CHUNK_WIDTH, image->width - xStart);    

            ImageChunk current;
            current.id = chunkId++;

            current.image.data = image->data + yStart * image->stride + xStart * 3;
            current.image.width = chunkWidth;
            current.image.height = chunkHeight;
            current.image.stride = image->stride;

            current.offsetX = xStart * SCALE_X + OFFSET_X;
            current.offsetY = yStart * SCALE_Y + OFFSET_Y;
            current.scaleX = SCALE_X;
            current.scaleY = SCALE_Y;

            chunks.push(current);
        }
    }

    if (this->config.verbosity >= Verbosity::Preparation)
    {
        std::cout << "Created " << chunkId << " chunks\n";
    }
}

bool ImageGenerator::allocateWork(Worker * worker)
{
    this->chunkQueueMutex.lock();
    this->allocCallCount++;

    bool ok = false;

    // simple version: allocate one work chunk to just the calling worker
    if (chunks.size() > 0)
    {
        ImageChunk chunk = chunks.front();
        chunks.pop();
        worker->addChunk(chunk);

        uint32_t yCoord = (chunk.image.data - origImage->data) / (3 * origImage->width);
        //std::cout << "Alloc " << yCoord << " to " << uint64_t(worker) << "\n";

        ok = true;
    }

    this->chunkQueueMutex.unlock();
    return ok;
}

void ImageGenerator::reportStats()
{
    std::cout << "Worker\tTime\tChunks\tWait time\n";

    // sums for stdev calculation
    double sum = 0;
    double sumSq = 0;

    uint64_t maxVal = 0;
    uint64_t minVal = 9999999999999;

    uint64_t waitTimeSum = 0;

    for (const Worker * w : this->workers)
    {
        const Worker::Stats stats = w->exitStats;
        std::cout << w->id << "\t" << stats.time << "\t" << stats.chunkCount << "\t" << stats.waitTime << "\n";

        waitTimeSum += stats.waitTime;

        sum += stats.time;
        sumSq += stats.time * stats.time;
        maxVal = std::max(maxVal, stats.time);
        minVal = std::min(minVal, stats.time);
    }

    const double n = this->workers.size();
    const double mean = sum / n;
    const double stDev = std::sqrt(sumSq / n - mean * mean);
    const double stDevPercent = 100 * stDev / maxVal;

    uint64_t peak2peak = maxVal - minVal;
    double peak2peakPercent = 100.0 * peak2peak / maxVal;

    std::cout << "Time std: " << stDev << " ms / " << stDevPercent << " %\n";
    std::cout << "Peak to peak: " << peak2peak << " ms / " << peak2peakPercent << " %\n";
    std::cout << "Total wait time: " << waitTimeSum / 1000 << " ms\n";
    std::cout << "Total calls to alloc function: " << this->allocCallCount << "\n";
}

ImageGenerator::ImageGenerator(Image * outputImage, const Config & config) : config(config)
{
    origImage = outputImage; // for debug
    chunkify(outputImage);
}

ImageGenerator::~ImageGenerator()
{
    for (uint32_t i = 0; i < workers.size(); i++)
    {
        delete workers[i];
    }
}

void ImageGenerator::run()
{
    // encapsulate the allocator function
    auto allocFunction = [&](Worker * worker) -> bool
    {
        return allocateWork(worker);
    };

    const uint32_t THREAD_COUNT = this->config.threadCount;

    // create workers and give each one initial chunk
    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
        Worker * worker = new Worker(i, allocFunction, this->config);
        
        ImageChunk chunk = chunks.front();
        chunks.pop();

        worker->addChunk(chunk);
        workers.push_back(worker);

        uint32_t yCoord = (chunk.image.data - origImage->data) / (3 * origImage->width);
        //std::cout << "Alloc " << yCoord << " to " << uint64_t(worker) << "\n";
    }

    // create array of threads so we can wait on them
    std::vector<std::thread> workerThreads;

    auto threadFunction = [](Worker * worker)
    {
        worker->run();
    };

    // start the workers
    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
        Worker * worker = workers[i];
        
        workerThreads.push_back(std::thread(threadFunction, worker));
    }

    // wait
    for (uint32_t i = 0; i < THREAD_COUNT; i++)
    {
        workerThreads[i].join();
    }

    if (this->config.verbosity >= Verbosity::Stats)
    {
        reportStats();
    }
}