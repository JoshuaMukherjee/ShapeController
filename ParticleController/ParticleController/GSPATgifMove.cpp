#include "ParticleControllerV2.hpp"
#include "ParticleControllerBEM.hpp"

#include "Helper.hpp"

void main() {
	int boardIDs[] = { 76,83 };
	
	float matBoardToWorld[] = {
		//top
		-1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0,-1, 0.24f,
		0, 0, 0, 1,

		-1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1,
	};


	const int N = 1;
	bool USE_DEVICE = true;

	ParticleControllerV2* controller = new ParticleControllerV2(N, boardIDs, matBoardToWorld);
	//ParticleController* controller = new ParticleController(N);

	ParticleControllerBEM* controllerBEM;
	if(!USE_DEVICE)
	{
		controllerBEM = new ParticleControllerBEM(N, boardIDs, matBoardToWorld);
		std::string reflector = "media/BEMFiles/flat-lam6.bin"; //Output of createReflector();
		controllerBEM->loadReflector(reflector);
	}
	

	float m[16 * N];
	getIdentity(N, m);
	//float A[] = { 0,-0.09, -0 }, B[] = { 0,-0.09f, 0.24 }, C[] = { 0,0.09f, 0 };
	int imgRes[] = { 256,256 };

	float max = 0.09;
	float min = 0.04;
	float pointPositions[] = { 0, min, 0.09 };
	float amp[] = {10000.f};

	//float A[] = { 0,0, 0.05 }, B[] = { 0,0, 0.13 }, C[] = { 0,max + 0.01, 0.05};
	float A[] = { 0,0, 0 }, B[] = { 0,0, 0.24 }, C[] = { 0,max + 0.01, 0 };


	if(USE_DEVICE){
		controller->updateParticle(pointPositions, 0);
		controller->UpdateBoard(m, m);
		controller->printPos(0);
	}
	printf("Press X to continue... \n");
	WaitforX();

	int frames = 60;
	char buffer[20];
	for (float pos = min; pos < max; pos += (max - min) / frames) {
		sprintf(buffer, "%f", pos);
		strcat(buffer, "fieldGSPAT.bmp");

		pointPositions[1] = pos;

		

		if(!USE_DEVICE){
			GSPAT::Solution* solution = controller->solver->createSolution(1, 1, true, pointPositions, amp, m, m);
			controller->solver->compute(solution);
			controllerBEM->Visualise(solution, buffer, A, B, C, imgRes, false);

			controller->solver->releaseSolution(solution);
		}
		else {
			controller->updateParticle(pointPositions,0);
			controller->UpdateBoard(m, m);
			Sleep(1000);
			printf("%f, %f, %f, \n", pointPositions[0], pointPositions[1], pointPositions[2]);
		}

		
	}

	printf("Done...");
	WaitforX();

	controller->close();




}
