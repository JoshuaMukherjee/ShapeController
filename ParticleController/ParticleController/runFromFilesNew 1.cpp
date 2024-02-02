#include <AsierInho.h>
#include <AsierInho_V2.h>
#include <GSPAT_SolverV2.h>
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <sstream>
#include <vector>

#include <stdlib.h>
#include <signal.h>

bool x;

BOOL WINAPI ConsoleHandler(DWORD dwType)
{
	switch (dwType) {
	case CTRL_C_EVENT:
		printf("ctrl-c\n");
		x = true;
		break;
	case CTRL_BREAK_EVENT:
		printf("break\n");
		break;
	default:
		printf("Some other event\n");
	}
	return TRUE;
}

bool USE_AMPLITUDE = false;

void printCSV(const char* str) {
	printf("%s\n", str);
}

void writePhases(std::string fileName, int numGeometries, int numTransducers, float* phases) {
	std::ofstream ofs;
	ofs.open(fileName);
	ofs << numGeometries << "," << numTransducers << std::endl;
	for (int g = 0; g < numGeometries; g++) {
		for (int t = 0; t < numTransducers; t++) {
			ofs << phases[g * numTransducers + t] << ",";
		}
		ofs << std::endl;
	}
	ofs.close();
}

void readTransducerInfo(std::string fileName, int& numGeometries, int& numTransducers, float*& phases) {
	std::ifstream ifs(fileName);
	if (ifs.is_open()) {
		std::string fileLine;
		bool firstLine = true;
		int currentGeometry = 0;
		bool reading = true;
		while (getline(ifs, fileLine) && reading) {
			std::stringstream toParse(fileLine);
			std::vector<std::string> lineTokens;
			std::string token;
			while (std::getline(toParse >> std::ws, token, ',')) {
				lineTokens.push_back(token);
			}
			if (!lineTokens.empty()) {
				if (firstLine) {
					numGeometries = atof(lineTokens[0].c_str());
					numTransducers = atof(lineTokens[1].c_str());
					phases = new float[numGeometries * numTransducers];
					firstLine = false;
				}
				else {
					for (int t = 0; t < numTransducers; t++) {
						float phase = atof(lineTokens[t].c_str());
						phases[currentGeometry * numTransducers + t] = phase;
					}
					currentGeometry++;
					//printf("read %d of %d geometries so far...\n", currentGeometry, numGeometries);
				}
				if (currentGeometry == numGeometries)
					reading = false;
			}
		}
	}
	else printf("ERROR: File cannot be opened!");
}

void fillPositionBuffer(std::string fileName, int& numGeometries, int& numPoints, float*& posBuffer) {
	const float heightOffset = 0.12f;
	std::ifstream ifs(fileName);
	if (ifs.is_open()) {
		std::string fileLine;
		bool firstLine = true;
		int currentLine = 0;
		bool reading = true;
		int posIndex = 0;
		while (getline(ifs, fileLine) && reading) {
			std::stringstream toParse(fileLine);
			std::vector<std::string> lineTokens;
			std::string token;
			while (std::getline(toParse >> std::ws, token, ',')) {
				lineTokens.push_back(token);
			}
			if (!lineTokens.empty()) {
				if (firstLine) {
					numGeometries = atof(lineTokens[0].c_str());
					numPoints = atof(lineTokens[1].c_str());
					posBuffer = new float[4 * numGeometries * numPoints];
					firstLine = false;
				}
				else {
					posBuffer[posIndex++] = atof(lineTokens[0].c_str());
					posBuffer[posIndex++] = atof(lineTokens[1].c_str());
					posBuffer[posIndex++] = atof(lineTokens[2].c_str()) + heightOffset;
					posBuffer[posIndex++] = 1;
					currentLine++;
					//printf("read %d of %d geometries so far...\n", currentGeometry, numGeometries);
				}
				if (currentLine == numGeometries * numPoints)
					reading = false;
			}
		}
	}
	else printf("ERROR: File cannot be opened!");
}

int main(void) {
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE)) {
		printf("Unable to install handler!\n");
		return EXIT_FAILURE;
	}

	AsierInho::AsierInhoBoard* disc = AsierInho::createAsierInho();
	disc->connect(AsierInho::BensDesign, 999, 1000);
	disc->disconnect();

	//Create handler and connect to it

	int numBoards = 2;
	int boardIDs[] = { 1000,999 }; //Could be the other way around?
	//float matBoardToWorld[] = {
	//	//topx
	//	-1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0,-1, 0.12f,
	//	0, 0, 0, 1,

	//	1, 0, 0, 0,
	//	0, 1, 0, 0,
	//	0, 0, 1, 0,
	//	0, 0, 0, 1,
	//};
	float matBoardToWorld[] = {
		//top
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0.24f,
		0, 0, 0, 1,

		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	int numTransducers = 256 * numBoards;


	AsierInho_V2::RegisterPrintFuncs(printCSV, printCSV, printCSV);
	AsierInho_V2::AsierInhoBoard_V2* driver = AsierInho_V2::createAsierInho();
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
	

	float* phaseBuffer, * amplitudeBuffer;
	int numGeometries;

	//bool simplyReadPhases = true;
	bool loop = false;
	printf("Running...");
	if (simplyReadPhases) {
		readTransducerInfo("JoshCSVs/TinySphere.csv", numGeometries, numTransducers, phaseBuffer);
	}

	unsigned char* messages = new unsigned char[2 * numGeometries * numTransducers];
	unsigned char phases_disc[512], amplitudes_disc[512];
	for (int g = 0; g < numGeometries; g++) {
		disc->discretizePhases(&(phaseBuffer[g * numTransducers]), phases_disc);
		if (USE_AMPLITUDE) {
			disc->discretizeAmplitudes(&(amplitudeBuffer[g * numTransducers]), amplitudes_disc);
			disc->correctPhasesShift(phases_disc, amplitudes_disc);
		}
		else {
			memset(amplitudes_disc, 64, numTransducers * sizeof(unsigned char));
		}
		memcpy(&messages[g * 2 * numTransducers + 0], &phases_disc[0], (numTransducers / 2) * sizeof(unsigned char));
		memcpy(&messages[g * 2 * numTransducers + numTransducers / 2], &amplitudes_disc[0], (numTransducers / 2) * sizeof(unsigned char));
		messages[g * 2 * numTransducers + 0] += 128;
		memcpy(&messages[g * 2 * numTransducers + numTransducers], &phases_disc[numTransducers / 2], (numTransducers / 2) * sizeof(unsigned char));
		memcpy(&messages[g * 2 * numTransducers + 3 * numTransducers / 2], &amplitudes_disc[numTransducers / 2], (numTransducers / 2) * sizeof(unsigned char));
		messages[g * 2 * numTransducers + numTransducers] += 128;
	}

	// simply update the first geometry
	for (int i = 0; i < 16; i++) {
		driver->updateMessage(&messages[0]);
	}

	printf("Press any key to move the particles\n");
	_getch();
	printf("Moving...\n");
	bool looping = true;
	while (!x && looping) {
		for (int g = 0; g < numGeometries; g++) {

			driver->updateMessage(&messages[2 * g * numTransducers]);
			//Sleep(10);
			looping = loop;
		}
	}
	printf("Moved\n");
	_getch();
	driver->turnTransducersOff();
	Sleep(100);
	driver->disconnect();
	delete driver;
	delete[] phaseBuffer, messages;
	if (USE_AMPLITUDE)
		delete[] amplitudeBuffer;
}