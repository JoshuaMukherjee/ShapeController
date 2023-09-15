#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"
#include <stdio.h>
#include <vector>



int main() {

	const int N = 2;
	ParticleController* Controller = new ParticleController(N);

	float m1[] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	float m[16 * N];
	for (int p = 0; p < N; p++)
		memcpy(&(m[16 * p]), m1, 16 * sizeof(float));

	float pointsA[][3] = {
		{0.01,-0.02,0.12},
		{0.02,-0.01,0.11},
		{0.02,0.01,0.12},
		{0.01,0.02,0.11},
		{-0.01,0.02,0.12},
		{-0.02,0.01,0.11},
		{-0.02,-0.01,0.12},
		{-0.01,-0.02,0.11},
		{0.01,-0.02,0.12}
	};

	float pointsB[][3] = {
		{-0.01,0.02,0.14},
		{-0.02,0.01,0.13},
		{-0.02,-0.01,0.14},
		{-0.01,-0.02,0.12},
		{0.01,-0.02,0.14},
		{0.01,-0.02,0.14},
		{0.02,-0.01,0.13},
		{0.02,0.01,0.14},
		{0.01,0.02,0.13}
	};

	Controller->updateParticle(pointsA[0], 0);
	Controller->updateParticle(pointsB[0], 1);
	Controller->printPos(0);
	Controller->printPos(1);
	Controller->UpdateBoard(m,m);
	
	printf("Starting");
	WaitforX();
	printf("Moving...");

	std::vector<float*> framesA = CreatePath(pointsA, 9, 500);
	std::vector<float*> framesB = CreatePath(pointsB, 9, 500);

	


	Controller->moveManyParticlesAlongFrames({framesA, framesB},m,m);

	printf("Done");
	WaitforX();

	Controller->close();
}