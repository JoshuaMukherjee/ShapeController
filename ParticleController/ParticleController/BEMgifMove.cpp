#include "ParticleControllerBEM.hpp"


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
	bool USE_DEVICE = false;
	int POINTS_PER_TRAP = 2;

	ParticleControllerBEM* controller = new ParticleControllerBEM(
		N, boardIDs, matBoardToWorld, 
		USE_DEVICE, 1, true, 2, 200, POINTS_PER_TRAP
	);

	std::string reflector = "media/BEMFiles/flat-lam6.bin"; //Output of createReflector();

	bool CREATE_REFLECTOR = false;
	if (CREATE_REFLECTOR) {
		std::string fIn = "media/BEMFiles/flat-lam6.stl"; //Input of createReflector();
		glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(0, 0.09, 0.12)) * glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));

		controller->createReflector(fIn, reflector, transform);
	};
	controller->loadReflector(reflector);

	float m[16 * N];
	getIdentity(N, m);
	
	int imgRes[] = { 256,256 };

	std::vector<glm::vec3> pointPositions(N);
	float max = 0.09;
	float min = 0.04;
	pointPositions[0] = glm::vec3(0, min, 0.09);

	//float A[] = { 0,0, 0.05 }, B[] = { 0,0, 0.13 }, C[] = { 0,max + 0.02, 0.05 };
    float A[] = { 0,0, 0 }, B[] = { 0,0, 0.24 }, C[] = { 0,max + 0.01, 0 };



	std::vector<float> targetAmplitudes(N);		// Pressure [Pa] for focal points (needs to be LARGER THAN ZERO!!)
	targetAmplitudes[0] = 10000;

	controller->setParticleLocations(pointPositions, targetAmplitudes);
	GSPAT::Solution* solution = controller->computeSolutionAndMove(m, m);
	controller->solver->releaseSolution(solution);
	printf("%f,%f,%f\n", pointPositions[0].x, pointPositions[0].y, pointPositions[0].z);

	int frames = 40;
	char buffer[20];
	WaitforX();
	for (float pos = min; pos < max; pos += (max - min) / frames) {
		sprintf(buffer, "%f", pos);
		strcat(buffer, "field.bmp");
		
		pointPositions[0].y = pos;
		controller->setParticleLocations(pointPositions, targetAmplitudes);

		solution = controller->computeSolutionAndMove(m, m);

		if(!USE_DEVICE){
			controller->Visualise(solution, buffer, A, B, C, imgRes, false);
		}
		else {
			printf("%f,%f,%f\n", pointPositions[0].x, pointPositions[0].y, pointPositions[0].z);
			Sleep(1000);
		}

		controller->solver->releaseSolution(solution);
		
	}

	printf("Done...");
	WaitforX();

	controller->close();




}
