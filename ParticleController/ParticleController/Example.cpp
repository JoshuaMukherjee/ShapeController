#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"
#include <stdio.h>
#include <vector>

#include <stdlib.h>
#include <signal.h>

int x = 0;


BOOL WINAPI ConsoleHandler(DWORD dwType)
{
	switch (dwType) {
	case CTRL_C_EVENT:
		printf("ctrl-c\n");
		x = 1;
		break;
	case CTRL_BREAK_EVENT:
		printf("break\n");
		break;
	default:
		printf("Some other event\n");
	}
	return TRUE;
}


int main() {

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE)) {
		printf("Unable to install handler!\n");
		return EXIT_FAILURE;
	}


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

	float** pointsA = copyArrayToPointers(points, 9);

	Controller->updateParticle(pointsA[0], 0);
	Controller->printPos(0);
	Controller->UpdateBoard(m,m);

	printf("Starting");
	WaitforX();
	printf("Moving...");

	std::vector<float*> frames = CreatePath(pointsA, 9, 500);
	while (!x) {
		Controller->moveParticleAlongFrames(frames, 0, m, m);
	}
	printf("Done");
	WaitforX();

	Controller->close();
}