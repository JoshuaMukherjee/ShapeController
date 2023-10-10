#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"
#include "Camera.hpp";



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
		//solver = GSPAT_V4::createSolver(512);

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



	void ParticleController::UpdateBoard(float m1[], float m2[]) {
		
		//printf("%f, %f, %f, %f, %f, %f, %f, %f,%f, %f, %f, %f \n", positions[0], positions[1], positions[2], positions[3], positions[4], positions[5], positions[6], positions[7], positions[8], positions[9], positions[10], positions[11]);
		GSPAT::Solution* solution = solver->createSolution(particleN, 1, false, positions, amplitude, m1, m2);

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

	void ParticleController::moveParticleAlongFrames(std::vector<float*> frames, int N, float m1[], float m2[]) {
		for (auto& frame : frames) {
			this->updateParticle(frame, N);
			this->printPos(N);
			this->UpdateBoard(m1,m2);
		}
	}

	void ParticleController::moveManyParticlesAlongFrames(std::vector<std::vector<float*>> particleFrames, float m1[], float m2[]) {
		float* frame;
		for (int i = 0; i < size(particleFrames.begin()[0]); i++) {
			for (int N = 0; N < size(particleFrames); N++) {
				//printf("%d,%d\n", i, N);
				frame = particleFrames.begin()[N][i];
				this->updateParticle(frame, N);
				this->printPos(N);
			}
			this->UpdateBoard(m1,m2);
		}
	}

	void ParticleController::moveManyParticlesAlongFrames(std::initializer_list<std::vector<float*>> particleFrames, float m1[], float m2[]) {
		std::vector<std::vector<float*>> v;
		for (std::vector<float*> i : particleFrames)
		{
			v.push_back(i);
		}
		moveManyParticlesAlongFrames(v,m1,m2);

	}

	void ParticleController::moveParticlesToStart(float target[][3], float* m, int N, float stageHeight, int f1, int f2) {

		std::vector<cv::Point3d> detectedPositions = GetParticleLocationsCamera(N, stageHeight);
		float particle[3];
		float tempPoint[3];

		std::vector< std::vector<float*> > frames;
		std::vector< std::vector<float*> > starters;

		for (int i = 0; i < N; i++) {
			particle[0] = detectedPositions[i].x;
			particle[1] = detectedPositions[i].y;
			particle[2] = stageHeight;

			tempPoint[0] = detectedPositions[i].x;
			tempPoint[1] = detectedPositions[i].y;
			tempPoint[2] = stageHeight + 0.02;

			this->updateParticle(particle, i);


			starters.push_back(CreateLine(particle, tempPoint, f1));
			frames.push_back(CreateLine(tempPoint, target[i], f2));

		};

		this->UpdateBoard(m, m);

		this->moveManyParticlesAlongFrames(starters, m, m);

		for (int j = 0; j < N; j++) {
			this->moveParticleAlongFrames(frames[j], j, m, m);
		}


	}


