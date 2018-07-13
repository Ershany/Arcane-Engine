#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "..\graphics\mesh\Mesh.h"
#include "..\graphics\Shader.h"
#include <SOIL.h>
#include <stb_image_aug.h>

#include "../graphics/mesh/Mesh.h"

namespace arcane { namespace terrain {

	class Terrain {
	private:
		float m_TerrainSize;
		unsigned int m_VertexSideCount;
		GLushort m_HeightMapScale;

		glm::mat4 m_ModelMatrix;
		glm::vec3 m_Position;
		graphics::Mesh *m_Mesh;
	public:
		Terrain(glm::vec3 &worldPosition);
		~Terrain();

		void Draw(graphics::Shader &shader) const;

		inline const glm::vec3& getPosition() const { return m_Position; }
	private:
		glm::vec3 calculateNormal(int x, int z, unsigned char *heightMapData);
		float getVertexHeight(int x, int y, unsigned char *heightMapData);
	};

} }

