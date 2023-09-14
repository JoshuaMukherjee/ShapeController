#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"
#include <stdio.h>
#include <vector>


int main() {
	ParticleController* Controller = new ParticleController(1);
	float points[][3] = {
		{0,0,0.12},
		{0,0.02,0.12},
		{0.01,0.02,0.12},
		{0,0.02,0.10}
	};

	Controller->updateParticle(points[0], 0);
	Controller->printPos(0);
	Controller->UpdateBoard();

	printf("Starting");
	WaitforX();
	printf("Moving...");
	std::vector<float*> frames = CreatePath(points, 4);
	Controller->moveParticleAlongFrames(frames, 0);

	printf("Done");
	WaitforX();

	Controller->close();
}