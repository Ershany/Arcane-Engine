#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

namespace arcane { namespace graphics {

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture {
		unsigned int id;
		std::string type;
	};

	class Mesh {
	public:
		// Mesh Data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> textures);
		void Draw(Shader &shader) const;
	private:
		unsigned int m_VAO, m_VBO, m_EBO;

		void setupMesh();
	};

} }