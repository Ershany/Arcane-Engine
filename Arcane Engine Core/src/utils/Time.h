#pragma once

#include <GLFW\glfw3.h>
#include <gl\GL.h>

namespace arcane {

	struct Time {
	private:
		GLdouble lastFrame;
		GLdouble delta;
	public:
		/**
		* Constructs an object that keeps track of the delta time since the last update
		* You must call the update function every update in order for it to properly keep track of the delta time
		*/
		Time();

		/**
		* To be called every update. It will keep track of the time since the last update
		*/
		void update();


		// Getters
		inline GLdouble getDeltaTime() const { return delta; }
	};

}