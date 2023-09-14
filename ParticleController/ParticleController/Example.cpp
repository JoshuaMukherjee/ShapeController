#include "ParticleController.hpp"
#include "Helper.hpp"
#include <stdio.h>


int main() {
	ParticleController* Controller = new ParticleController(1);
	float pos[] = { -0.01,-0.01,0.12 };
	Controller->updateParticle(pos, 0);
	Controller->printPos(0);
	Controller->UpdateBoard();
	WaitforX();
	printf("Moving...");
	for (int i = 0; i < 100; i++) {
		pos[1] += 0.0001;
		Controller->updateParticle(pos, 0);
		Controller->UpdateBoard();
	}
	printf("Done");
	WaitforX();
	Controller->close();
}