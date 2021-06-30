#include "arcpch.h"
#include "Time.h"

namespace Arcane
{
	Time::Time() {
		lastFrame = glfwGetTime();
		delta = 0;
	}

	void Time::update() {
		delta = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();
	}
}
