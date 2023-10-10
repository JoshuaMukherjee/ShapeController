#include "ParticleControllerBEM.hpp"


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

	ParticleControllerBEM* controller = new ParticleControllerBEM(N, boardIDs, matBoardToWorld);

	std::string reflector = "media/BEMFiles/flat-lam4.bin"; //Output of createReflector();

	bool CREATE_REFLECTOR = false;
	if(CREATE_REFLECTOR){
		std::string fIn = "media/BEMFiles/flat-lam4.stl"; //Input of createReflector();
		glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(0, 0.08, 0.12)) * glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));

		controller->createReflector(fIn, reflector, transform);
	};
	controller->loadReflector(reflector);

	float m[16 * N];
	getIdentity(N, m);

	std::vector<glm::vec3> pointPositions(N);
	pointPositions[0] = glm::vec3(0, 0.02f, 0.07f);

	std::vector<float> targetAmplitudes(N);		// Pressure [Pa] for focal points (needs to be LARGER THAN ZERO!!)
	targetAmplitudes[0] = 10000;

	controller->setParticleLocations(pointPositions, targetAmplitudes);

	GSPAT::Solution* solution = controller->computeSolutionAndMove(m, m);
	float A[] = { 0,-0.09, -0 }, B[] = { 0,-0.09f, 0.24 }, C[] = { 0,0.09f, 0 };
	int imgRes[] = { 256,256 };
	
	controller->Visualise(solution, "amplitude.bmp", A, B, C, imgRes);
	
	controller->solver->releaseSolution(solution);
	
	WaitforX();

	controller->close();




}
