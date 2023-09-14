#include "Shapes.hpp";
#include <Windows.h>

std::vector<float*> CreateLine(float* start, float* end, int steps) {

	std::vector<float*> frames;

	float distanceX = end[0] - start[0];
	float distanceY = end[1] - start[1];
	float distanceZ = end[2] - start[2];

	float dX = distanceX / steps;
	float dY = distanceY / steps;
	float dZ = distanceZ / steps;

	printf("%f, %f, %f \n", distanceX, distanceY, distanceZ);

	frames.push_back(start);
	
	float* prev;
	for (int i = 0; i < steps; i++) {
		float* frame = new float[3];
		prev = frames.back();
		frame[0] = prev[0] + dX;
		frame[1] = prev[1] + dY;
		frame[2] = prev[2] + dZ;

		frames.push_back(frame);
	}

	return frames;

}

std::vector<float*> CreatePath(float points[][3],int NFrames ,int steps) {
	float* start;
	float* end;
	std::vector<float*> frames;
	std::vector<float*> tempFrames;

	for (int i = 0; i < NFrames - 1; i++) {
		start = points[i];
		end = points[i + 1];
		printf("%f,%f,%f -->", start[0], start[1], start[2]);
		printf("%f,%f,%f\n", end[0], end[1], end[2]);
		tempFrames = CreateLine(start, end, steps / NFrames);
		frames.insert(frames.end(), tempFrames.begin(), tempFrames.end());
	}
	return frames;
}