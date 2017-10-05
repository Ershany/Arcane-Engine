#pragma once

#include <glm/glm.hpp>
#include <assimp\Importer.hpp>
#include <string>
#include <vector>

#include "Shader.h"

namespace arcane { namespace graphics {

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;

		Vertex() {}
		Vertex(glm::vec3 &pos, glm::vec3 &normal, glm::vec2 &texCoords) {
			Position = pos; Normal = normal; TexCoords = texCoords;
		}
		Vertex(float xPos, float yPos, float zPos, float xNorm, float yNorm, float zNorm, float xTexCoord, float yTexCoord) {
			Position.x = xPos; Position.y = yPos; Position.z = zPos;
			Normal.x = xNorm; Normal.y = yNorm; Normal.z = zNorm;
			TexCoords.x = xTexCoord; TexCoords.y = yTexCoord;
		}
	};

	struct Texture {
		unsigned int id;
		std::string type;
		aiString path; // Allows us to compare with other textures so no duplicate textures are generated. TODO change to String
	};

	class Mesh {
	public:
		Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures);
		void Draw(Shader &shader) const;

		// Getters
		inline const std::vector<Vertex>& getVertices() const { return m_Vertices; }
		inline const std::vector<unsigned int>& getIndices() const { return m_Indices; }
		inline const std::vector<Texture>& getTextures() const { return m_Textures; }
	private:
		unsigned int m_VAO, m_VBO, m_EBO;

		// Mesh Data
		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
		std::vector<Texture> m_Textures;

		void setupMesh();
	};

} }