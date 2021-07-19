#include "arcpch.h"
#include "Timer.h"

namespace Arcane
{
	Timer::Timer() {
		m_StartTime = glfwGetTime();
	}

	void Timer::Reset() {
		m_StartTime = glfwGetTime();
	}

	void Timer::Rewind(double time) {
		m_StartTime += time;
		if (Elapsed() < 0.0) {
			m_StartTime = glfwGetTime();
		}
	}

	double Timer::Elapsed() const {
		return glfwGetTime() - m_StartTime;
	}
};
