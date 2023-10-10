//#pragma once

//#include <glm/glm.hpp>
#include <GSPAT_SolverBEM.h>
#include <AsierInho_V2.h>
#include <stdio.h>
#include <BEMSolver.h>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <ReflectionSurface.h>
#include "CImg\CImg.h"

#include "Helper.hpp"

class ParticleControllerBEM {
	//These are parameters you can play with
	bool USE_DEVICE;			// Actually send the data to a acoustophoretic device or not 
	int numPoints;			// Number of points (trap/focus) 
	int numGeometries;		// It's okay to fix this to 1 in this main file
	bool phaseOnly;		// Phase only modulation or Phase+Amplitude modulation
	int numIterations;			// Number of iterations in the solver (more iteration, it gets better but slower)
	int numPointsPerTrap;		// 1: Create focal points using WGS, 2 or 4: Create traps by minimizing Gor'kov potential
	float targetPotential;	// Target Gor'kov potential at each point
	//Set up your points
	std::vector<glm::vec3> pointPositions;	// (x, y, z)
	//pointPositions[1] = glm::vec3(0.f, -0.02f, 0.04f);
	std::vector<float> targetAmplitudes;		// Pressure [Pa] for focal points (needs to be LARGER THAN ZERO!!)
	//targetAmplitudes[1] = 10000;
	//Parameters for your system
	int numBoards;
	int* boardIDs;
	float* matBoardToWorld;
	int numTransducers;

	float* positionBuffer;
	float* targetBuffer;
public:
	AsierInho_V2::AsierInhoBoard_V2* driver;
	GSPAT::Solver* solver;



public:
	ParticleControllerBEM( //Constructor
		int numPointsIn,
		int* boardIDsIn,
		float* matBoardToWorldIn,
		bool USE_DEVICEIn = false,
		int numGeometriesIn = 1,
		bool phaseOnlyIn = true,
		int numBoardsIn = 2,
		int numIterationsIn = 200,
		int numPointsPerTrapIn = 2,
		float targetPotentialIn = -4e-7
	);

	void createReflector(std::string fileIn, std::string fileOut, glm::mat4 transform);
	void loadReflector(std::string file);
	void close();
	void setParticleLocations(std::vector<glm::vec3> pointPositions, std::vector<float> targetAmplitudes);
	GSPAT::Solution* computeSolutionAndMove(float* m1, float* m2);
	void Visualise(GSPAT::Solution* solution, char* fout, float A[], float B[], float C[], int imageSize[], bool show = true);

};

