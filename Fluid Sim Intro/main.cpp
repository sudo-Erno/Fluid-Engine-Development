#include <iostream>
#include <cstdio>
#include <array>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <thread>

#define M_PI 3.14159265358979323846264338327950288
using namespace std;
using namespace chrono;

const size_t kBufferSize = 80;
const char* kGrayScaleTable = " .:-=+*#%@";
const size_t kGrayScaleTableSize = sizeof(kGrayScaleTable) / sizeof(char);

void updateWave(const double timeInterval, double* x, double* speed) {
    (*x) += timeInterval * (*speed);

    // Boundary reflection
    if ((*x) > 1.0) {
        (*speed) *= -1.0;
        (*x) = 1.0 + timeInterval * (*speed);
    } else if ((*x) < 0.0) {
        (*speed) *= -1.0;
        (*x) = timeInterval * (*speed);
    }
}

void accumulateWaveToHeightField(const double x, const double wavelength, const double maxHeight, array<double, kBufferSize>* heightField) {
    const double quarterWavelength = 0.25 * wavelength;
    const int start = static_cast<int>((x - quarterWavelength) * kBufferSize);
    const int end = static_cast<int>((x + quarterWavelength) * kBufferSize);

    for (int i = start; i < end; i++) {
        int iNew = i;
        if (i < 0) {
            iNew = -i - 1;
        } else if(i >= static_cast<int>(kBufferSize)) {
            iNew = 2 * kBufferSize - i - 1;
        }

        double distance = fabs((i + 0.5) / kBufferSize - x);
        double height = maxHeight * 0.5 * (cos(min(distance * M_PI / quarterWavelength, M_PI)) + 1.0);

        (*heightField)[iNew] += height;
    }
}

void draw(const array<double, kBufferSize>& heightField) {
    string buffer(kBufferSize, ' ');

    // Convert height field to grayscale
    for (size_t i = 0; i < kBufferSize; i++) {
        double height = heightField[i];
        size_t tableIndex = min(static_cast<size_t>(floor(kGrayScaleTableSize * height)), kGrayScaleTableSize - 1);
        buffer[i] = kGrayScaleTable[tableIndex];
    }

    // Clear old prints
    for (size_t i = 0; i < kBufferSize; i++) {
        printf("\b");
    }

    // Draw new buffer
    printf("%s", buffer.c_str());
    fflush(stdout);
}

int main()
{
    const double waveLengthX = 0.8;
    const double waveLengthY = 1.2;

    const double maxHeightX = 0.5;
    const double maxHeightY = 0.4;

    double x = 0.0;
    double y = 1.0;
    double speedX = 1.0;
    double speedY = -0.5;


    const int fps = 100;
    const double timeInterval = 1.0 / fps;

    array<double, kBufferSize> heightField;

    for (int i = 0; i < 1000; i++) {
        // Update waves
        updateWave(timeInterval, &x, &speedX);
        updateWave(timeInterval, &y, &speedY);

        // Clear height field
        for (double& height: heightField) {
            height = 0.0;
        }

        // Accumulate waves for each center point
        accumulateWaveToHeightField(x, waveLengthX, maxHeightX, &heightField);
        accumulateWaveToHeightField(y, waveLengthY, maxHeightY, &heightField);

        // Draw height field
        draw(heightField);

        // Wait
        this_thread::sleep_for(milliseconds(1000 / fps));
    }

    printf("\n");
    fflush(stdout);

    return 0;
}
