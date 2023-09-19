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
		printf( "Unable to install handler!\n");
		return EXIT_FAILURE;
	}



	const int N = 2;
	ParticleController* Controller = new ParticleController(N);
	printf("Starting...\n");


	float m[16 * N];
	getIdentity(N, m);

	float p1[] = { 0.04, 0.03, 0.1};
	float p2[] = { -0.04, 0.02, 0.1};
	float p3[] = { 0, 0.01, 0.14 };
	std::vector<float*> circleFramesA = CreatePolygon(9, p1, p2, p3, 7000);

	float p1b[] = { 0.01,-0.04, 0.1 };
	float p2b[] = { -0.01, -0.02, 0.1 };
	float p3b[] = { 0.01, -0.01, 0.14 };
	std::vector<float*> circleFramesB = CreatePolygon(9, p1b, p2b, p3b, 7000);

	Controller->updateParticle(circleFramesA[0], 0);
	Controller->updateParticle(circleFramesB[0], 1);
	Controller->printPos(0);
	Controller->printPos(1);
	Controller->UpdateBoard(m, m);

	WaitforX();
	printf("Moving...\n");


	while (x == 0) {
		Controller->moveManyParticlesAlongFrames({ circleFramesA, circleFramesB }, m, m);
	}
	printf("Done");
	WaitforX();


	Controller->close();
}