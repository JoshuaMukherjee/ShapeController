#include "ParticleController.hpp"
#include "ParticleControllerBEM.hpp"

#include "Helper.hpp"

void main() {
	int boardIDs[] = { 76,83 };

	float matBoardToWorld[] = {
		//top
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0,-1, 0.12f,
		0, 0, 0, 1,

		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	const int N = 1;

	ParticleController* controller = new ParticleController(N);
	ParticleControllerBEM* controllerBEM = new ParticleControllerBEM(N, boardIDs, matBoardToWorld);
	std::string reflector = "media/BEMFiles/flat-lam4.bin"; //Output of createReflector();
	controllerBEM->loadReflector(reflector);


	float m[16 * N];
	getIdentity(N, m);
	/*float A[] = { 0,-0.09, -0 }, B[] = { 0,-0.09f, 0.24 }, C[] = { 0,0.09f, 0 };*/
	int imgRes[] = { 256,256 };

	float max = 0.08;
	float min = 0.04;
	float pointPositions[] = { 0, min, 0.09 };
	float amp[] = {10000.f};

	float A[] = { 0,0, 0.05 }, B[] = { 0,0, 0.13 }, C[] = { 0,max + 0.01, 0.05};


	int frames = 30;
	char buffer[20];
	for (float pos = min; pos < max; pos += (max - min) / frames) {
		sprintf(buffer, "%f", pos);
		strcat(buffer, "fieldGSPAT.bmp");

		pointPositions[1] = pos;

		GSPAT::Solution* solution = controller->solver->createSolution(1, 1, true, pointPositions, amp, m, m);
		controller->solver->compute(solution);

		controllerBEM->Visualise(solution, buffer, A, B, C, imgRes, false);

		controller->solver->releaseSolution(solution);
	}

	printf("Done...");
	WaitforX();

	controller->close();




}
