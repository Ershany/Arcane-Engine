#pragma once
#ifndef QUAD_H
#define QUAD_H

#ifndef MESH_H
#include <Arcane/Graphics/Mesh/Mesh.h>
#endif

namespace Arcane
{
	class Quad : public Mesh {
	public:
		Quad(bool ndcQuad = true);
	};
}
#endif
