#pragma once
#include <AsierInho.h>
#include <stdio.h>
#include <GSPAT_SolverV2.h>
#include <conio.h>
#include <Windows.h>

class ParticleController {
	int particleN;
	float* positions;
	float* amplitude;
	GSPAT::Solver* solver;
	AsierInho::AsierInhoBoard* driver;
public:
	ParticleController(int particleNin, int B1 = 83, int B2 = 76);
	void updateParticle(float* pos, int N);
	void printPos(int N);
	void UpdateBoard();
	void close();
	void moveParticleAlongFrames(std::vector<float*> frames, int N);
};