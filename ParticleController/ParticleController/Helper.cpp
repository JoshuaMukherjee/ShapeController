#include "Helper.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>


void WaitforX() {
	bool finished = false;
	while (!finished) {
		switch (_getch()) {
		case 'x':
		case 'X':
			finished = true;
			break;
		}
	}
}

void printArray(float data[]) {
	using namespace std;
	for (int i = sizeof(data) / sizeof(data[0]); i >= 0; i--) {
		printf("%f ", data[i]);
	}
	printf("\n");
}

void getIdentity(int N, float* m) {
	float m1[] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	for (int p = 0; p < N; p++)
		memcpy(&(m[16 * p]), m1, 16 * sizeof(float));

}

float** copyArrayToPointers(float array[][3], int size) {
	float** A_ptr = (float**)malloc(sizeof(float*) * size);
	for (int i = 0; i < size; i++)
		A_ptr[i] = (float*)malloc(sizeof(double) * 3);

	// copy matrix
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 3; j++) {
			A_ptr[i][j] = array[i][j];
			//printf(" %f ", A_ptr[i][j]);
		}
	}
	return A_ptr;
}

std::vector<float> cross_product(std::vector<float> vec1, std::vector<float> vec2) {
	std::vector<float> out;
	out.push_back(vec1[1] * vec2[2] - vec1[2] * vec2[1]);
	out.push_back(vec1[2] * vec2[0] - vec1[0] * vec2[2]);
	out.push_back(vec1[0] * vec2[1] - vec1[1] * vec2[0]);
	return out;
}

float magnitude(std::vector<float> vec) {
	float mag = 0;
	for (auto i : vec){
		mag += pow(i, 2);
	}
	return sqrt(mag);
}

std::vector<float> addVec(std::vector<float> vec1, std::vector<float> vec2) {
	std::vector<float> result;
	for (int i = 0; i < size(vec1); i++) {
		result.push_back(vec1[i] + vec2[i]);
	}
	return result;
}

std::vector<float> subVec(std::vector<float> vec1, std::vector<float> vec2) {
	std::vector<float> result;
	for (int i = 0; i < size(vec1); i++) {
		result.push_back(vec1[i] - vec2[i]);
	}
	return result;
}

std::vector<float> scalerMultiply(std::vector<float> vec, float scalar) {
	for (int i = 0; i < size(vec); i++) {
		vec[i] = vec[i] * scalar;
	}

	return vec;
}

float distance(std::vector<float> vec1, std::vector<float> vec2) {
	float dist = 0;
	for (int i = 0; i < size(vec1); i++) {
		dist += pow(vec2[i] - vec1[i],2);
	}
	return sqrt(dist);
}

void printVec(std::vector<float> vec) {
	for (auto i : vec) {
		printf("%f ", i);
	}
	printf("\n");
}
