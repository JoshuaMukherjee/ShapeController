#include "Helper.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>


void WaitforX() {
	bool finished = false;
	while (!finished) {
		switch (_getch()) {
		case 'x':
		case 'X':
			finished = true;
			break;
		}
	}
}

 

void printArray(float data[]) {
	using namespace std;
	for (int i = sizeof(data) / sizeof(data[0]); i >= 0; i--) {
		printf("%f ", data[i]);
	}
	printf("\n");
}