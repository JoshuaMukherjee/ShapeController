#include "Helper.hpp"


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