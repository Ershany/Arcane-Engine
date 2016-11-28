#pragma once

#include <GLFW\glfw3.h>
#pragma once

namespace arcane {

	class Timer {
	private:
		double startTime;
	public:
		Timer();
		void reset();
		double elapsed();
	};

} 