#pragma once

#include <Arcane/Graphics/Mesh/Mesh.h>

namespace Arcane
{
	class Quad : public Mesh {
	public:
		Quad(bool ndcQuad = true);
	};
}
