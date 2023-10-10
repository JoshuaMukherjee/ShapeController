#include "ParticleController.hpp"
#include "Helper.hpp"
#include "Shapes.hpp"

void print(const char* str) {
	printf("%s\n", str);
}


void pickManyBeads() {
	const int N = 4;
	float stageHeight = 0.042;

	ParticleController* Controller = new ParticleController(N);

	float m[16 * N];
	getIdentity(N, m);

	float targets[][3] = { 
		{-0.03,0.03,0.12},
		{0.03,-0.03,0.12},
		{-0.03,-0.03,0.12},
		{0.03,0.03,0.12}
	};

	Controller->moveParticlesToStart(targets, m, N);

	WaitforX();

	Controller->close();
}


int main() {
	//multiparticleDetectExample();
	pickManyBeads();


}