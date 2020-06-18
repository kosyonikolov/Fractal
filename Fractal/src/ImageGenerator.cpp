#include "ImageGenerator.h"
#include "generateImage.h"

#include <cmath>

void Worker::run()
{
    if (!this->quiet)
    {
        std::cout << "W" << this->id << ": Start\n";
    }
    //std::cout << "Start " << uint64_t(this) << "\n";
    auto start = std::chrono::steady_clock::now();

    Stats stats;
    stats.chunkCount = 0;

    while (true)
    {
        // check if there is a chunk to work on
        if (chunks.size() == 0)
        {
            if (allocateWork == 0)
            {
                // no chunks and no allocator function - time to exit
                break;
            }

            // call the allocator to see if it gives us work
            bool ok = allocateWork(this);
            if (!ok)
            {
                break;
            }
        }

        // if we are here then there is a chunk to work on
        ImageChunk current = chunks.front();
        chunks.pop();

        if (!this->quiet)
        {
            std::cout << "W" << this->id << ": Starting work on chunk " << current.id << "\n";
        }

        generateImage(&current.image,
                      current.offsetX, current.offsetY, current.scaleX, current.scaleY,
                      this->maxPixelIterations);

        stats.chunkCount++;
    }

    auto end = std::chrono::steady_clock::now();
    stats.time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    this->exitStats = stats;

    if (!this->quiet)
    {
        std::cout << "W" << this->id << ": Stop\n";
    }
}

void Worker::addChunk(const ImageChunk & chunk)
{
    chunks.push(chunk);
}

void ImageGenerator::chunkify(const Image * image,
                              const double offsetX, const double offsetY,
                              const double scaleX, const double scaleY,
                              const uint32_t count)
{
    // make each chunk span the whole image length-wise
    const uint32_t chunkHeight = image->height / count;
    uint32_t leftover = image->height % count;

    uint32_t imgY = 0;

    // create chunks
    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t currentHeight = chunkHeight;
        // first chunks get to be larger
        if (leftover > 0)
        {
            currentHeight++;
            leftover--;
        }

        const uint32_t imgX = 0;

        ImageChunk current;
        current.id = i;

        current.image.data = image->data + imgY * image->stride;
        current.image.width = image->width;
        current.image.height = currentHeight;
        current.image.stride = image->stride;

        current.offsetX = imgX * scaleX + offsetX;
        current.offsetY = imgY * scaleY + offsetY;
        current.scaleX = scaleX;
        current.scaleY = scaleY;

        chunks.push(current);
        imgY += currentHeight;
    }

    // sanity check
    if (imgY != image->height)
    {
        throw std::runtime_error("Sanity check FAILED: summed image height is not equal to real height!\n");
    }
}

bool ImageGenerator::allocateWork(Worker * worker)
{
    this->chunkQueueMutex.lock();

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
    std::cout << "Worker\tTime\tChunks\n";

    // sums for stdev calculation
    double sum = 0;
    double sumSq = 0;

    uint64_t maxVal = 0;

    for (const Worker * w : this->workers)
    {
        const Worker::Stats stats = w->getExitStats();
        std::cout << w->id << "\t" << stats.time << "\t" << stats.chunkCount << "\n";

        sum += stats.time;
        sumSq += stats.time * stats.time;
        maxVal = std::max(maxVal, stats.time);
    }

    const double n = this->workers.size();
    const double mean = sum / n;
    const double stDev = std::sqrt(sumSq / n - mean * mean);
    const double stDevPercent = 100 * stDev / maxVal;

    std::cout << "Time std: " << stDev << " ms / " << stDevPercent << " %\n";
}

ImageGenerator::ImageGenerator(Image * outputImage,
                               const double offsetX, const double offsetY,
                               const double scaleX, const double scaleY,
                               const uint32_t maxIters,
                               const uint32_t threadCount, const uint32_t granularity,
                               const bool quiet) : threadCount(threadCount), maxIters(maxIters), isQuiet(quiet)
{
    origImage = outputImage; // for debug
    const uint32_t chunkCount = threadCount * granularity;
    chunkify(outputImage, offsetX, offsetY, scaleX, scaleY, chunkCount);
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

    // create workers and give each one initial chunk
    for (uint32_t i = 0; i < threadCount; i++)
    {
        Worker * worker = new Worker(i, this->maxIters, allocFunction, this->isQuiet);
        
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
    for (uint32_t i = 0; i < threadCount; i++)
    {
        Worker * worker = workers[i];
        
        workerThreads.push_back(std::thread(threadFunction, worker));
    }

    // wait
    for (uint32_t i = 0; i < threadCount; i++)
    {
        workerThreads[i].join();
    }

    if (!this->isQuiet)
    {
        reportStats();
    }
}