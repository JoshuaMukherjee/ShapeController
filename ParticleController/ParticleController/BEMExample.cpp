#include "ParticleControllerBEM.hpp"


void main() {
	int boardIDs[] = { 999 }; //83x

	float matBoardToWorld[] = {
		//top
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0,-1, 0.24f,
		0, 0, 0, 1

		//1, 0, 0, 0,
		//0, 1, 0, 0,
		//0, 0, 1, 0,
		//0, 0, 0, 1,
	};

	const int N = 1;

	ParticleControllerBEM* controller = new ParticleControllerBEM(N, boardIDs, matBoardToWorld, false, 1,true,1);

	std::string reflector = "media/BEMFiles/Sphere-lam2.bin"; //Output of createReflector();
	
	bool CREATE_REFLECTOR = 0;
	if(CREATE_REFLECTOR){
		std::string fIn = "media/BEMFiles/Sphere-lam2.stl"; //Input of createReflector();
		glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(0, -0.06, 0.04f)) * glm::rotate(glm::mat4(1.f), glm::radians(0.0f), glm::vec3(0, 0,1 ));

		controller->createReflector(fIn, reflector, transform);
		//Will probably crash, just run again without doing this bit
		return;
	};
	controller->loadReflector(reflector);

	float m[16 * N];
	getIdentity(N, m);

	std::vector<glm::vec3> pointPositions(N);
	pointPositions[0] = glm::vec3(0, 0, 0.12f);

	std::vector<float> targetAmplitudes(N);		// Pressure [Pa] for focal points (needs to be LARGER THAN ZERO!!)
	targetAmplitudes[0] = 10000;

	controller->setParticleLocations(pointPositions, targetAmplitudes);

	GSPAT::Solution* solution = controller->computeSolutionAndMove(m, m);
	float A[] = { -0.12f,0, 0.24 }, B[] = { 0.12f,0, 0.24 }, C[] = { -0.12f,0, 0 };
	int imgRes[] = { 256,256 };
	
	controller->Visualise(solution, "amplitude.bmp", A, B, C, imgRes, true);
	
	controller->solver->releaseSolution(solution);
	
	WaitforX();

	controller->close();




}
