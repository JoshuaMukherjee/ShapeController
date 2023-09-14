#pragma once
#include <vector>

std::vector<float*> CreateLine(float* start, float* end, int steps = 100);
std::vector<float*> CreatePath(float points[][3], int NFrames, int steps = 500);