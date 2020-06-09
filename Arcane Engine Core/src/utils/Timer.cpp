#include "pch.h"
#include "Timer.h"

namespace arcane {

	Timer::Timer() {
		m_StartTime = glfwGetTime();
	}

	void Timer::reset() {
		m_StartTime = glfwGetTime();
	}

	void Timer::rewind(double time) {
		m_StartTime += time;
		if (elapsed() < 0.0) {
			m_StartTime = glfwGetTime();
		}
	}

	double Timer::elapsed() const {
		return glfwGetTime() - m_StartTime;
	}

};
