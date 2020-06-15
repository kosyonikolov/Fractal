#include "ImageGenerator.h"
#include "generateImage.h"

Worker::Stats Worker::run()
{
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

        generateImage(current.image, current.width, current.height, current.stride,
                      current.offsetX, current.offsetY, current.scaleX, current.scaleY,
                      this->maxPixelIterations, this->lut);

        stats.chunkCount++;
    }

    auto end = std::chrono::steady_clock::now();
    stats.time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return stats;
}

void Worker::addChunk(const ImageChunk & chunk)
{
    chunks.push(chunk);
}