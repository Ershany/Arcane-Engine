#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#ifndef MESH_H
#include <Arcane/Graphics/Mesh/Mesh.h>
#endif

namespace Arcane
{
	class Sphere : public Mesh {
	public:
		Sphere(int xSegments = 30, int ySegments = 30);
	};
}
#endif
