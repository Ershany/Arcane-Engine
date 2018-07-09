#pragma once

#include <GL/glew.h>

namespace arcane { namespace graphic {

	class PostProcessor {
	public:
		PostProcessor();
	public:
		unsigned int texture;

		// Post Processing Toggles
		bool blur = false;
	};

} }