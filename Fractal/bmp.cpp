#include <cstdio>
#include "bmp.h"

int saveBmpRgb(const std::string& filename, const uint8_t* image, const int width, const int height)
{
    FILE* f;
    f = fopen(filename.c_str(), "wb");
    if (f == NULL)
    {
        perror("Unable to open file");
        return -1;
    }
    const int FILE_HEADER_LEN = 14;
    const int INFO_HEADER_LEN = 40;
    const int HEADER_LEN = FILE_HEADER_LEN + INFO_HEADER_LEN;
    const int filesize = HEADER_LEN + 3 * width * height;

    uint8_t bmpfileheader[FILE_HEADER_LEN] = { 'B','M', 0,0,0,0, 0,0, 0,0, HEADER_LEN,0,0,0 };
    uint8_t bmpinfoheader[INFO_HEADER_LEN] = { INFO_HEADER_LEN,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
    uint8_t bmppad[3] = { 0,0,0 };

    bmpfileheader[2] = (uint8_t)(filesize);
    bmpfileheader[3] = (uint8_t)(filesize >> 8);
    bmpfileheader[4] = (uint8_t)(filesize >> 16);
    bmpfileheader[5] = (uint8_t)(filesize >> 24);

    bmpinfoheader[4] = (uint8_t)(width);
    bmpinfoheader[5] = (uint8_t)(width >> 8);
    bmpinfoheader[6] = (uint8_t)(width >> 16);
    bmpinfoheader[7] = (uint8_t)(width >> 24);
    bmpinfoheader[8] = (uint8_t)(height);
    bmpinfoheader[9] = (uint8_t)(height >> 8);
    bmpinfoheader[10] = (uint8_t)(height >> 16);
    bmpinfoheader[11] = (uint8_t)(height >> 24);

    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);
    for (int i = 0; i < height; i++)
    {
        fwrite(image + (width * (height - i - 1) * 3), 3, width, f);
        fwrite(bmppad, 1, (4 - (width * 3) % 4) % 4, f);
    }
    fclose(f);
}