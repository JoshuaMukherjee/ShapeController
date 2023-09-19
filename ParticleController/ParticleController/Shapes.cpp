#include "Shapes.hpp";
#include "Helper.hpp"
#include <Windows.h>
#include <cmath>


std::vector<float*> CreateLine(float* start, float* end, int steps) {

	std::vector<float*> frames;

	float distanceX = *end - *start;
	float distanceY = *(end+1) - *(start+1);
	float distanceZ = *(end+2) - *(start+2);

	float dX = distanceX / steps;
	float dY = distanceY / steps;
	float dZ = distanceZ / steps;

	//printf("%f, %f, dist %f, %f, %f \n", *start, *end, distanceX, distanceY, distanceZ);

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

std::vector<float*> CreatePath( float** points,int NFrames ,int steps) {

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


float** GetPolygonPoints(int sides, float* start, float* p2, float* p3) {
	float** pointsbuffer = (float**)calloc(sides+1, sizeof(float*));
	for (int i = 0; i < sides+1; i++) { pointsbuffer[i] = (float*)calloc(3, sizeof(float)); }


	std::vector<float> point1(start, start + 3);
	std::vector<float> point2(p2, p2 + 3);
	std::vector<float> point3(p3, p3 + 3);


	std::vector<float> a = subVec(point1, point3);
	std::vector<float> b = subVec(point2, point3);

	float radius = (magnitude(a) * magnitude(b) * magnitude(subVec(a, b))) / (2 * magnitude(cross_product(a, b)));
	std::vector<float> tempa = scalerMultiply(b, pow(magnitude(a), 2));
	std::vector<float> tempb = scalerMultiply(a, pow(magnitude(b), 2));
	std::vector<float> tempc = subVec(tempa, tempb);
	std::vector<float> top = cross_product(tempc, cross_product(a, b));
	std::vector<float> tempd = cross_product(a, b);
	float bottom = pow(magnitude(cross_product(a, b)), 2) * 2;
	std::vector<float> centreTemp = scalerMultiply(top, 1 / bottom);
	std::vector<float> centre = addVec(centreTemp, point3);
	//printVec(centre);

	std::vector<float> topN = cross_product(subVec(point2, point1), subVec(point3, point1));
	float bottomN = 1/magnitude(cross_product(subVec(point2, point1), subVec(point3, point1)));
	std::vector<float> n = scalerMultiply(topN, bottomN);

	std::vector<std::vector<float>> points;
	points.reserve(sides);

	for (float r = 0; r <= 3.1415*2; r += 3.1415 * 2 / sides) {
		std::vector<float> p =	addVec(centre, 
								addVec(	scalerMultiply(subVec(centre, point1), cos(r)), 
										scalerMultiply(cross_product(n, subVec(centre, point1)), sin(r))));
		
		points.push_back(p);
		printVec(p);

	}
	printf("\n\n");

	for (int i = 0; i < sides+1; i++) {
		pointsbuffer[i][0] = points[i][0];
		pointsbuffer[i][1] = points[i][1];
		pointsbuffer[i][2] = points[i][2];

	}


	return pointsbuffer;

}

std::vector<float*> CreatePolygon(int sides, float* start, float* p2, float* p3, int steps) {
	


	float** points = GetPolygonPoints(sides, start, p2,p3);

	std::vector<float*> frames = CreatePath(points, sides+1, steps/(sides+1));
	return frames;

}


//void main() {
//
//	float p1[] = { 0, 0.03,0.12 };
//	float p2[] = { 0, -0.03 , 0.12 };
//	float p3[] = { 0.03, 0, 0.12 };
//
//	//float** points = GetPolygonPoints(5, p1, p2, p3);
//	std::vector<float*> circleFrames = CreatePolygon(5, p1, p2, p3, 500);
//
//
//	
//}