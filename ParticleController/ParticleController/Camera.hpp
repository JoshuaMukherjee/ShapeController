#pragma once
#include <vector>
#include <BeadDetector.h>


std::vector<cv::Point3d> GetParticleLocationsCamera(int N, float stageHeight, float corner=0.084);
std::vector<cv::Point3d> getNPoints(BeadDetector& detector, int N);