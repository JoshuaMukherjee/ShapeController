#include "ParticleControllerBEM.hpp"


void print(const char* str) {
	printf("%s\n", str);
}


ParticleControllerBEM::ParticleControllerBEM(
	int numPointsIn,
	int* boardIDsIn,
	float* matBoardToWorldIn,
	bool USE_DEVICEIn,
	int numGeometriesIn,
	bool phaseOnlyIn,
	int numBoardsIn,
	int numIterationsIn,
	int numPointsPerTrapIn,
	float targetPotentialIn) {


	//These are parameters you can play with
	USE_DEVICE = USE_DEVICEIn;			// Actually send the data to a acoustophoretic device or not 
	numPoints = numPointsIn;			// Number of points (trap/focus) 
	numGeometries = numGeometriesIn;		// It's okay to fix this to 1 in this main file
	phaseOnly = phaseOnlyIn;		// Phase only modulation or Phase+Amplitude modulation
	numIterations = numIterationsIn;			// Number of iterations in the solver (more iteration, it gets better but slower)
	numPointsPerTrap = numPointsPerTrapIn;			// 1: Create focal points using WGS, 2 or 4: Create traps by minimizing Gor'kov potential
	targetPotential = targetPotentialIn;		// Target Gor'kov potential at each point
	//Set up your points
	std::vector<glm::vec3> pointPositions(numPoints);	// (x, y, z)
	pointPositions[0] = glm::vec3(0, 0.f, 0.f);
	std::vector<float> targetAmplitudes(numPoints);		// Pressure [Pa] for focal points (needs to be LARGER THAN ZERO!!)
	targetAmplitudes[0] = 10000;
	//Parameters for your system
	numBoards = numBoardsIn;
	boardIDs = (int*)malloc(sizeof(int) * numBoards);
	memcpy(boardIDs, boardIDsIn, sizeof(int) * numBoards);
	matBoardToWorld = (float*)malloc(16 * sizeof(float) * numBoards);
	memcpy(matBoardToWorld, matBoardToWorldIn, 16 * sizeof(int) * numBoards);
	numTransducers = numBoards * 256;

	positionBuffer = (float*) calloc(4 * numGeometries * numPoints, sizeof(float));
	targetBuffer = (float*) calloc(numGeometries * numPoints, sizeof(float));

	//Create driver and connect to it
	AsierInho_V2::RegisterPrintFuncs(print, print, print);
	driver = AsierInho_V2::createAsierInho();
	if (!driver->connect(numBoards, boardIDs, matBoardToWorld))
		printf("Failed to connect to board.");
	//Read parameters to be used for the solver
	float* transducerPositions = new float[numTransducers * 3];
	float* transducerNormals = new float[numTransducers * 3];
	float* amplitudeAdjust = new float[numTransducers];
	int* mappings = new int[numTransducers];
	int* phaseDelays = new int[numTransducers];
	int numDiscreteLevels;
	driver->readParameters(transducerPositions, transducerNormals, mappings, phaseDelays, amplitudeAdjust, &numDiscreteLevels);
	//Create solver:
	GSPAT_BEM::RegisterPrintFuncs(print, print, print);
	solver = GSPAT_BEM::createSolver(numTransducers);//Number of transducers used (two boards of 16x16)
	solver->setBoardConfig(transducerPositions, transducerNormals, mappings, phaseDelays, amplitudeAdjust, numDiscreteLevels);
	//Tune the algorithm
	GSPAT::Solver::ConfigParameter algorithmConfig[3];
	algorithmConfig[0] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::NUM_ITERATIONS, (void*)numIterations };
	algorithmConfig[1] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::NUM_POINTS_PER_TRAP, (void*)numPointsPerTrap };
	algorithmConfig[2] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::TARGET_POTENTIAL, (void*)(&targetPotential) };
	solver->setConfigParameters(3, algorithmConfig);
	//Delete parameters as they won't be used anymore
	delete[] transducerPositions, transducerNormals, amplitudeAdjust, mappings, phaseDelays;
}

void ParticleControllerBEM::createReflector(std::string fileIn, std::string fileOut, glm::mat4 transform) {
	ReflectionSurface reflector;
	//glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(0, 0.08, 0.12)) * glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
	//glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, 0));
	reflector.addSTLObject(fileIn, transform);
	//reflector.addSTLObject("media/BEMFiles/STL/extension-lam6.stl", translate);
	//reflector.addSTLObject("media/BEMFiles/STL/XYbigwavy-lam4.stl", translate);
	//reflector.addSTLObject("media/BEMFiles/STL/0.06-0.16extension-lam2.stl", translate);
	int numMeshes = reflector.getNumMeshes();
	float* meshPositions = reflector.getCentres();
	float* meshAreas = reflector.getAreas();
	float* meshNormals = reflector.getNormals();
	// Compute Matrices you need to compute the transducer-to-mesh matrix
	float* matrixA, * matrixB;
	GSPAT_BEM::computeMatrixAandB(solver, numMeshes, meshPositions, meshAreas, meshNormals, matrixA, matrixB);
	// We don't need the driver and sovler any more, so delete them
	driver->disconnect();
	delete driver;
	delete solver;
	// Create BEM solver and compute your transducer-to-mesh matrix
	BEMSolver* bemSolver = new BEMSolver();
	bemSolver->setConfiguration(numMeshes, numTransducers, matrixA, matrixB, false);
	bemSolver->decomposeMatrixA();
	float* transducer2MeshMatrix = bemSolver->solveMatrixT2M();
	// Export the necessary files
	BEMSolver::exportBEMFile(fileOut, numTransducers, numMeshes, meshPositions, meshAreas, meshNormals, transducer2MeshMatrix);
	// Delete the buffers
	delete[] matrixA, matrixB;
	delete bemSolver;
	delete[] meshPositions, meshAreas, meshNormals;
	delete[] transducer2MeshMatrix;
}




void ParticleControllerBEM::loadReflector(std::string file){
	//Set up your reflector
	int numTransducersTmp, numMeshes;
	float* meshPositions, * meshAreas, * meshNormals, * t2mMatrix;
	BEMSolver::importBEMFile(file, numTransducersTmp, numMeshes, meshPositions, meshAreas, meshNormals, t2mMatrix);
	if (numTransducers != numTransducersTmp) { printf("WARNING::the file containing reflector information seems wrong\n"); return; }
	GSPAT::Solver::ConfigParameter reflectorConfig[5];
	reflectorConfig[0] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::REFLECTOR_NUM_ELEMENTS, (void*)numMeshes };
	reflectorConfig[1] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::REFLECTOR_ELEMENT_POSITIONS, (void*)meshPositions };
	reflectorConfig[2] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::REFLECTOR_ELEMENT_AREAS, (void*)meshAreas };
	reflectorConfig[3] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::REFLECTOR_ELEMENT_NORMALS, (void*)meshNormals };
	reflectorConfig[4] = GSPAT::Solver::ConfigParameter{ GSPAT_BEM::TRANSDUCER_TO_REFLECTOR_MATRIX, (void*)t2mMatrix };
	solver->setConfigParameters(5, reflectorConfig);
	delete[] meshPositions; delete[] meshAreas; delete[] meshNormals;
	delete[] t2mMatrix;
}

void ParticleControllerBEM::close() {
	free(boardIDs);
	free(matBoardToWorld);
	free(positionBuffer);
	free(targetBuffer);

	driver->turnTransducersOff();
	Sleep(100);
	driver->disconnect();
	delete driver;
	delete solver;

}

void ParticleControllerBEM::setParticleLocations(std::vector<glm::vec3> pointPositions, std::vector<float> targetAmplitudes) {
	for (int g = 0; g < numGeometries; g++) {
		for (int p = 0; p < numPoints; p++) {
			positionBuffer[4 * g * numPoints + 4 * p + 0] = pointPositions[p].x;
			positionBuffer[4 * g * numPoints + 4 * p + 1] = pointPositions[p].y;
			positionBuffer[4 * g * numPoints + 4 * p + 2] = pointPositions[p].z;
			positionBuffer[4 * g * numPoints + 4 * p + 3] = 1;
			targetBuffer[g * numPoints + p] = targetAmplitudes[p];
		}
	}
}

GSPAT::Solution* ParticleControllerBEM::computeSolutionAndMove(float* m1, float* m2) {
	//let's compute a solution and update it to the board connected
	GSPAT::Solution* solution = solver->createSolution(numPoints, numGeometries, phaseOnly, positionBuffer, targetBuffer, m1, m2);
	solver->compute(solution);
	unsigned char* msg;
	solution->finalMessages(&msg);
	if (USE_DEVICE) {
		for (int s = 0; s < 16; s++)//Fill FPGA buffers so update is processed directly
			driver->updateMessage(msg);
	}

	return solution;
}

void ParticleControllerBEM::Visualise(GSPAT::Solution* solution, char* fout, float A[], float B[], float C[], int imageSize[], bool show) {

	float* hologram = solution->finalHologramReIm();

	cimg_library::CImg<unsigned char> img = cimg_library::CImg<unsigned char>(imageSize[0], imageSize[1], 1, 3);
	//1.Initialize iterator vectors.
	float step_AB[3], step_AC[3];
	step_AB[0] = (B[0] - A[0]) / imageSize[0]; //Step vector in direction AB
	step_AB[1] = (B[1] - A[1]) / imageSize[0];
	step_AB[2] = (B[2] - A[2]) / imageSize[0];
	step_AC[0] = (C[0] - A[0]) / imageSize[1]; //Step vector in direction AC
	step_AC[1] = (C[1] - A[1]) / imageSize[1];
	step_AC[2] = (C[2] - A[2]) / imageSize[1];
	//2. Step through the pixels:
	int numFieldPoints = imageSize[0] * imageSize[1];
	float* fieldPositions = new float[4 * numFieldPoints];
	int positionIndex = 0;
	for (int px = 0; px < imageSize[0]; px++) {
		for (int py = 0; py < imageSize[1]; py++) {
			//2.1. Compute 3D position of pixel (px,py)
			fieldPositions[positionIndex++] = A[0] + px * step_AB[0] + py * step_AC[0];
			fieldPositions[positionIndex++] = A[1] + px * step_AB[1] + py * step_AC[1];
			fieldPositions[positionIndex++] = A[2] + px * step_AB[2] + py * step_AC[2];
			fieldPositions[positionIndex++] = 1;
		}
	}
	float* field = GSPAT_BEM::simulateFieldFromHologram(solver, hologram, numFieldPoints, fieldPositions);

	float* amplitudes = new float[imageSize[0] * imageSize[1]];
	float avg_Amp = 0, min_Amp = 1000000, max_Amp = 0;;
	for (int px = 0; px < imageSize[0]; px++) {
		for (int py = 0; py < imageSize[1]; py++) {
			float Re = field[2 * (px * imageSize[1] + py) + 0];
			float Im = field[2 * (px * imageSize[1] + py) + 1];
			//2.3. Map field to pixel colour: 
			float amp_R = sqrt(Re * Re + Im * Im);
			amplitudes[py * imageSize[0] + px] = amp_R;
			//Some stats:
			avg_Amp += amp_R;
			min_Amp = (amp_R < min_Amp ? amp_R : min_Amp);
			max_Amp = (amp_R > max_Amp ? amp_R : max_Amp);
		}
	}
	float ampToColour = 3 * 256 / max_Amp;
	for (int px = 0; px < imageSize[0]; px++) {
		for (int py = 0; py < imageSize[1]; py++) {

			unsigned int amp_R = (unsigned int)(ampToColour * amplitudes[py * imageSize[0] + px]);
			unsigned int R_aux = 0, G_aux = 0, B_aux = 0;
			if (amp_R < 256) R_aux = amp_R;
			else if (amp_R < 512) { R_aux = 255; G_aux = (amp_R - 256); }
			else { R_aux = 255; G_aux = 255; B_aux = amp_R - 512; }
			unsigned char R = (unsigned char)(R_aux);
			unsigned char G = (unsigned char)(G_aux);
			unsigned char B = (unsigned char)(B_aux);
			img(px, py, 0) = R; img(px, py, 1) = G; img(px, py, 2) = B;
		}
	}

	printf(fout);
	img.save_bmp(fout);
	if (show) {
		static cimg_library::CImgDisplay disp(img, "Playing with images...", false);
		disp.assign(img);
	}
	delete[] field;
	delete[] amplitudes;
	printf("\nAVG Amp= %f;\n MIN Amp=%f;\n MAX amp=%f;\n", avg_Amp / (imageSize[0] * imageSize[1]), min_Amp, max_Amp);
}