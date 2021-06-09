#include <vector>
#include <limits.h>
#include "utils.hpp"
using namespace std;

const int imageWidth = 512;
const int imageHeght = 512;


int getNewValue(vector<vector<int>> pixels, vector<vector<int>> mask)
{
    int sum = 0;
    for(auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 3; j++) {
            sum += pixels[i][j] * mask[i][j];
        }
    }
    return sum;
}


short *bufferWithBorder = NULL;
void createBufferWithBorder()
{
    int width = imageWidth + 2;
    int height = imageHeght + 2;
    bufferWithBorder = new short[width * height];
    for(auto i = 1; i < height - 1; i++) {
        for (auto j = 1; j < width - 1; j++) {
            bufferWithBorder[i * width + j] = buffer[(i - 1) * imageWidth + j - 1];
        }
        bufferWithBorder[i * width] = buffer[(i - 1) * imageWidth];
        bufferWithBorder[(i + 1) * width - 1] = buffer[i * imageWidth - 1];
        bufferWithBorder[i] = buffer[i - 1];
        bufferWithBorder[width * height - i - 1] = buffer[imageHeght * imageWidth - i];
    }
    bufferWithBorder[0] = buffer[0];
    bufferWithBorder[width * height - 1] = buffer[imageWidth * imageHeght - 1];
    bufferWithBorder[width - 1] = buffer[imageWidth - 1];
    bufferWithBorder[(width - 1) * height] = buffer[(imageWidth - 1) * imageHeght];
}

void filtration(vector<vector<int>> mask)
{
    int width = imageWidth + 2;
    int height = imageHeght + 2;
    short *currentBuffer = new short[imageHeght * imageWidth];
    for(auto i = 1; i < height - 1; i++) {
        for (auto j = 1; j < width - 1; j++) {
            vector<vector<int>> pixels = {
                {
                    bufferWithBorder[(i - 1) * width + j - 1],
                    bufferWithBorder[(i - 1) * width + j],
                    bufferWithBorder[(i - 1) * width + j + 1]
                },
                {
                    bufferWithBorder[i * width + j - 1],
                    bufferWithBorder[i * width + j],
                    bufferWithBorder[i * width + j + 1]
                },
                {
                    bufferWithBorder[(i + 1) * width + j - 1],
                    bufferWithBorder[(i + 1) * width + j],
                    bufferWithBorder[(i + 1) * width + j + 1]
                }
            };
            currentBuffer[(i - 1) * imageWidth + j - 1] = getNewValue(pixels, mask);
        }
    }
    short max = 0;
    short min = SHRT_MAX;
    for (auto i = 0; i < imageWidth * imageHeght; i++) {
        max = currentBuffer[i] > max ? currentBuffer[i] : max;
        min = currentBuffer[i] < min ? currentBuffer[i] : min;
    }
    for (auto i = 0; i < imageWidth * imageHeght; i++) {
        currentBuffer[i] = double(currentBuffer[i]) / (max - min) * SHRT_MAX;
    }
}


