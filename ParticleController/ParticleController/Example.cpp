#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"
#include <stdio.h>
#include <vector>



int main() {

	ParticleController* Controller = new ParticleController(1);
	float m[] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	float points[][3] = {
		{0.01,-0.02,0.12},
		{0.02,-0.01,0.11},
		{0.02,0.01,0.12},
		{0.01,0.02,0.13},
		{-0.01,0.02,0.12},
		{-0.02,0.01,0.11},
		{-0.02,-0.01,0.12},
		{-0.01,-0.02,0.13},
		{0.01,-0.02,0.12}
	};

	Controller->updateParticle(points[0], 0);
	Controller->printPos(0);
	Controller->UpdateBoard(m,m);

	printf("Starting");
	WaitforX();
	printf("Moving...");

	std::vector<float*> frames = CreatePath(points, 9, 500);

	Controller->moveParticleAlongFrames(frames, 0,m,m);

	printf("Done");
	WaitforX();

	Controller->close();
}