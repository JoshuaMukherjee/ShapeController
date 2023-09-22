#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"

#include <BeadDetector.h>

void print(const char* str) {
	printf("%s\n", str);
}


void multiparticleDetectExample() {

	const int N = 3;
	float stageHeight = 0.042;

	ParticleController* Controller = new ParticleController(N);

	float m[16 * N];
	getIdentity(N, m);

	float targets[][3] = { {0,0,0.12},
					 {-0.03,0,0.12},
					 {0.03,0,0.12} };
	
	Controller->moveParticlesToStart(targets, m, N);
	
	printf("Done");
	WaitforX();

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

	std::vector<float*> framesA = CreatePath((float**)pointsA, 5, 500);
	std::vector<float*> framesB = CreatePath((float**)pointsB, 5, 500);
	std::vector<float*> framesC = CreatePath((float**)pointsC, 5, 500);
	
	printf("Moving...\n");


	Controller->moveManyParticlesAlongFrames({ framesA, framesB, framesC }, m, m);


	WaitforX();

	Controller->close();

}

int main() {
	multiparticleDetectExample();
}