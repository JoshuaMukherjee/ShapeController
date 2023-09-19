#pragma once
#include <vector>

std::vector<float*> CreateLine(float* start, float* end, int steps = 100);
std::vector<float*> CreatePath(float** points, int NFrames, int steps = 500);
std::vector<float*> CreatePolygon(int sides, float* start, float* p2, float* p3, int steps = 500);