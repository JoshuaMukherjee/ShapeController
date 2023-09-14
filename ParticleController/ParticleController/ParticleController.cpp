#include "ParticleController.hpp"


ParticleController::ParticleController(int particleNin, int B1, int B2) {
		particleN = particleNin;
		positions = new float[4 * particleN];
		amplitude = new float[particleN];
		for (int i = 0; i < particleN; i++) {
			amplitude[i] = 10000;
		}
		
		driver = AsierInho::createAsierInho();
		if (!driver->connect(AsierInho::BensDesign, B1, B2))
			printf("Failed to connect to board.");

		solver = GSPAT_V2::createSolver(512);

		float transducerPositions[512 * 3], transducerNormals[512 * 3], amplitudeAdjust[512];
		int mappings[512], phaseDelays[512], numDiscreteLevels;
		driver->readParameters(transducerPositions, mappings, phaseDelays, amplitudeAdjust, &numDiscreteLevels);
		for (int i = 0; i < 512; i++) {
			transducerNormals[3 * i + 0] = 0.0f;
			transducerNormals[3 * i + 1] = 0.0f;
			transducerNormals[3 * i + 2] = 1.0f;
		}
		solver->setBoardConfig(transducerPositions, transducerNormals, mappings, phaseDelays, amplitudeAdjust, numDiscreteLevels);
	}

void ParticleController::updateParticle(float* pos, int N) {
		positions[4 * N] = pos[0];
		positions[4 * N + 1] = pos[1];
		positions[4 * N + 2] = pos[2];
		positions[4 * N + 3] = 1;

	}

	void ParticleController::printPos(int N) {
		printf("Particle %d at %f, %f, %f \n", N, positions[4 * N], positions[4 * N + 1], positions[4 * N + 2]);
	}

	void ParticleController::UpdateBoard(){
		float m1[] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		GSPAT::Solution* solution = solver->createSolution(particleN, 1, false, positions, amplitude, m1, m1);

		solver->compute(solution);
		unsigned char* msg;
		solution->finalMessages(&msg);
		for (int s = 0; s < 16; s++) {//Fill FPGA buffers so update is processed directly
			driver->updateMessage(msg);
		}
		solver->releaseSolution(solution);
	}

	void ParticleController::close() {
		driver->turnTransducersOff();
		Sleep(100);
		driver->disconnect();
		delete driver;
		delete solver;
	}

	void ParticleController::moveParticleAlongFrames(std::vector<float*> frames, int N) {
		for (auto& frame : frames) {
			this->updateParticle(frame, N);
			this->printPos(0);
			this->UpdateBoard();
		}
	}

