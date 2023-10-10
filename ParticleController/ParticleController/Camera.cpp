#include "Camera.hpp";
#include "Helper.hpp";


std::vector<cv::Point3d> GetParticleLocationsCamera(int N, float stageHeight, float corner) {

	float     p1[] = { -corner, -corner,  stageHeight },
		p2[] = { corner, -corner, stageHeight },
		p3[] = { corner,  corner, stageHeight },
		p4[] = { -corner,  corner, stageHeight };

	BeadDetector& detector = BeadDetector::instance(p1, p2, p3, p4);
	detector.startDetection();
	std::vector<cv::Point3d> detectedPositions = getNPoints(detector, N);

	return detectedPositions;
}

std::vector<cv::Point3d> getNPoints(BeadDetector& detector, int N) {
	static const int minThres = 10, maxThres = 160, thresStep = 10;
	int threshold = minThres;
	printf("Place Bead...\n");
	WaitforX();
	while (1) {
		detector.setThreshold(threshold);
		std::vector<cv::Point3d> currentBeads = detector.getCurrentBeads();
		if (currentBeads.size() == N) {
			return currentBeads;
		}
		if (threshold == maxThres) threshold = minThres;
		else threshold += thresStep;
	}
}