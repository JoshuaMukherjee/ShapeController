#pragma once
#include <AsierInho_V2.h>
#include <stdio.h>
#include <GSPAT_SolverV2.h>
#include <GSPAT_SolverIBP.h>
#include <conio.h>
#include <Windows.h>
#include <stdio.h>
#include <BeadDetector.h>


class ParticleControllerV2 {
	int particleN;
	float* positions;
	float* amplitude;
	AsierInho_V2::AsierInhoBoard_V2* driver;
	int numBoards;
	float* matBoardToWorld;
	int* boardIDs;
	int numTransducers;
public:
	GSPAT::Solver* solver;

	ParticleControllerV2(int particleNin, int* boardIDsIn, float* matBoardToWorldIn,int numBoardsIn=2);
	void updateParticle(float* pos, int N);
	void printPos(int N);
	void UpdateBoard(float m1[], float m2[]);
	void close();
	void moveParticleAlongFrames(std::vector<float*> frames, int N, float m1[], float m2[]);
	void moveManyParticlesAlongFrames(std::initializer_list<std::vector<float*>> particleFrames, float m1[], float m2[]);
	void moveManyParticlesAlongFrames(std::vector<std::vector<float*>> particleFrames, float m1[], float m2[]);
	void moveParticlesToStart(float target[][3], float* m, int N, float stageHeight = 0.042, int f1 = 500, int f2 = 1000);
};

