#include "Timer.h"

namespace arcane {

	Timer::Timer() {
		startTime = glfwGetTime();
	}

	void Timer::reset() {
		startTime = glfwGetTime();
	}

	double Timer::elapsed() {
		return glfwGetTime() - startTime;
	}

};

