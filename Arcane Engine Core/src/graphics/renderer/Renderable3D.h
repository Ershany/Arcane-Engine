#pragma once

#include "../Model.h"
#include <glm\glm.hpp>

namespace arcane { namespace graphics {

	class Renderable3D {
	public:
		Renderable3D(glm::vec3 &position, Model *model);
		~Renderable3D();

		void draw(Shader &shader) const;
	private:
		glm::vec3 m_Position;
		
		Model *m_Model;
	};

} }