#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"
#include <stdio.h>
#include <vector>



int main() {

	const int N = 3;
	ParticleController* Controller = new ParticleController(N);

	
	float m[16 * N];
	getIdentity(N, m);

	float pointsAarray[][3] = {
		{0,0,0.12},
		{0,0.02,0.12},
		{0,0.02,0.09},
		{0,0,0.09},
		{0,0,0.12}
	};

	float pointsBarray[][3] = {
		{-0.03,0,0.12},
		{-0.03,0.02,0.12},
		{-0.03,0.02,0.09},
		{-0.03,0,0.09},
		{-0.03,0,0.12}
	};

	float pointsCarray[][3] = {
		{0.03,0,0.12},
		{0.03,0.02,0.12},
		{0.03,0.02,0.09},
		{0.03,0,0.09},
		{0.03,0,0.12}
	};

	float** pointsA = copyArrayToPointers(pointsAarray, 5);
	float** pointsB = copyArrayToPointers(pointsBarray, 5);
	float** pointsC = copyArrayToPointers(pointsCarray, 5);

	
	Controller->updateParticle(pointsA[0], 0);
	Controller->updateParticle(pointsB[0], 1);
	Controller->updateParticle(pointsC[0], 2);
	Controller->printPos(0);
	Controller->printPos(1);
	Controller->printPos(2);
	Controller->UpdateBoard(m,m);
	
	printf("Starting...\n");

	std::vector<float*> framesA = CreatePath((float**) pointsA, 5, 500);
	std::vector<float*> framesB = CreatePath((float**) pointsB, 5, 500);
	std::vector<float*> framesC = CreatePath((float**) pointsC, 5, 500);
	WaitforX();
	printf("Moving...\n");


	
	Controller->moveManyParticlesAlongFrames({ framesA, framesB, framesC }, m, m);

	printf("Done");
	WaitforX();
	

	Controller->close();
}