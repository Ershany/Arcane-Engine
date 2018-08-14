#pragma once

#include "../Mesh.h"

namespace arcane { namespace graphics {

	class Sphere : public Mesh {
	public:
		Sphere(int xSegments = 30, int ySegments = 30);
	};

} }