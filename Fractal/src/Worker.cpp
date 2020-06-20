#include "Worker.h"

#include <chrono>
#include <iostream>

#include "generateImage.h"

void Worker::run()
{
    if (this->config.verbosity >= Verbosity::Full)
    {
        std::cout << "W" << this->id << ": Start\n";
    }
    auto start = std::chrono::steady_clock::now();

    Stats stats;
    stats.chunkCount = 0;
    stats.waitTime = 0;

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

            auto waitStart = std::chrono::steady_clock::now();
            // call the allocator to see if it gives us work
            bool ok = allocateWork(this);
            auto waitEnd = std::chrono::steady_clock::now();
            stats.waitTime += std::chrono::duration_cast<std::chrono::microseconds>(waitEnd - waitStart).count();

            if (!ok)
            {
                break;
            }
        }

        // if we are here then there is a chunk to work on
        ImageChunk current = chunks.front();
        chunks.pop();

        if (this->config.verbosity >= Verbosity::Full)
        {
            std::cout << "W" << this->id << ": Starting work on chunk " << current.id << "\n";
        }

        auto chunkStart = std::chrono::steady_clock::now();

        generateImage(&current.image,
                      current.offsetX, current.offsetY, current.scaleX, current.scaleY,
                      this->config.maxIterations);

        auto chunkEnd = std::chrono::steady_clock::now();
        ProcessedChunk processed;
        ImageChunk & p = processed;
        p = current;
        processed.time = std::chrono::duration_cast<std::chrono::milliseconds>(chunkEnd - chunkStart).count();

        this->processedChunks.push_back(processed);

        stats.chunkCount++;
    }

    auto end = std::chrono::steady_clock::now();
    stats.time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    this->exitStats = stats;

    if (this->config.verbosity >= Verbosity::Full)
    {
        std::cout << "W" << this->id << ": Stop\n";
    }
}

void Worker::addChunk(const ImageChunk & chunk)
{
    chunks.push(chunk);
}