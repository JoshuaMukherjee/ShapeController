#pragma once
#include <conio.h>
#include <vector>



void WaitforX();

void printArray(float a[]);

void getIdentity(int N, float* m);

float** copyArrayToPointers(float array[][3], int size);

std::vector<float> cross_product(std::vector<float> vec1, std::vector<float> vec2);

float magnitude(std::vector<float> vec);

std::vector<float> addVec(std::vector<float> vec1, std::vector<float> vec2);

std::vector<float> subVec(std::vector<float> vec1, std::vector<float> vec2);

std::vector<float> scalerMultiply(std::vector<float> vec, float scalar);

std::vector<float> scalerMultiply(std::vector<float> vec, float scalar);

float distance(std::vector<float> vec1, std::vector<float> vec2);

void printVec(std::vector<float> vec);