#include "helpers.h"
#include <math.h>
#include <string.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE pixel = image[i][j];
            float average = (pixel.rgbtBlue + pixel.rgbtGreen + pixel.rgbtRed) / (float)3;
            pixel.rgbtBlue = (uint8_t)round(average);
            pixel.rgbtGreen = (uint8_t)round(average);
            pixel.rgbtRed = (uint8_t)round(average);
            image[i][j] = pixel;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        int startPointer = 0;
        int endPointer = width - 1;
        while (startPointer < endPointer)
        {
            RGBTRIPLE pixel = image[i][startPointer];
            RGBTRIPLE mPixel = image[i][endPointer];
            image[i][startPointer] = mPixel;
            image[i][endPointer] = pixel;
            startPointer++;
            endPointer--;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE tmp[height][width];
    memcpy(tmp, image, sizeof(RGBTRIPLE) * height * width);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumRed = 0;
            int sumGreen = 0;
            int sumBlue = 0;
            int boxSize = 0;
            int startingPixelX = j - 1;
            int startingPixelY = i - 1;
            int endPixelX = j + 1;
            int endPixelY = i + 1;

            for (int x = startingPixelX; x <= endPixelX; x++)
            {
                for (int y = startingPixelY; y <= endPixelY; y++)
                {
                    if (x >= 0 && x < width && y >= 0 && y < height)
                    {
                        boxSize++;
                        sumRed += tmp[y][x].rgbtRed;
                        sumGreen += tmp[y][x].rgbtGreen;
                        sumBlue += tmp[y][x].rgbtBlue;
                    }
                }
            }

            RGBTRIPLE pixel = image[i][j];
            pixel.rgbtBlue =  round(sumBlue / (float)boxSize);
            pixel.rgbtGreen = round(sumGreen / (float)boxSize);
            pixel.rgbtRed =   round(sumRed / (float)boxSize);
            image[i][j] = pixel;
        }
    }
    return;
}

int min(int x, int y)
{
    if (x < y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

int max(int x, int y)
{
    if (x > y)
    {
        return x;
    }
    return y;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    RGBTRIPLE tmp[height][width];
    memcpy(tmp, image, sizeof(RGBTRIPLE) * height * width);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            long sumRedX = 0;
            long sumGreenX = 0;
            long sumBlueX = 0;
            long sumRedY = 0;
            long sumGreenY = 0;
            long sumBlueY = 0;
            int boxSize = 0;
            int startingPixelX = j - 1;
            int startingPixelY = i - 1;
            int endPixelX = j + 1;
            int endPixelY = i + 1;

            for (int x = startingPixelX, gx = 0; x <= endPixelX; x++, gx++)
            {
                for (int y = startingPixelY, gy = 0; y <= endPixelY; y++, gy++)
                {
                    if (x >= 0 && x < width && y >= 0 && y < height)
                    {
                        boxSize++;
                        sumRedX += (tmp[y][x].rgbtRed * Gx[gx][gy]);
                        sumGreenX += (tmp[y][x].rgbtGreen * Gx[gx][gy]);
                        sumBlueX += (tmp[y][x].rgbtBlue * Gx[gx][gy]);

                        sumRedY += (tmp[y][x].rgbtRed * Gy[gx][gy]);
                        sumGreenY += (tmp[y][x].rgbtGreen * Gy[gx][gy]);
                        sumBlueY += (tmp[y][x].rgbtBlue * Gy[gx][gy]);
                    }
                }
            }

            RGBTRIPLE pixel = image[i][j];
            pixel.rgbtBlue = min(max(round(sqrt(pow(sumBlueX, 2) + pow(sumBlueY, 2))), 0), 255);
            pixel.rgbtGreen = min(max(round(sqrt(pow(sumGreenX, 2) + pow(sumGreenY, 2))), 0), 255);
            pixel.rgbtRed =  min(max(round(sqrt(pow(sumRedX, 2) + pow(sumRedY, 2))), 0), 255);
            image[i][j] = pixel;
        }
    }
    return;
}
