#include "Mesh.h"

#include <sstream>

namespace arcane { namespace graphics {

	Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> textures) {
		this->m_Vertices = vertices;
		this->m_Indices = indices;
		this->m_Textures = textures;

		setupMesh();
	}

	void Mesh::setupMesh() {
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}

	void Mesh::Draw(Shader &shader) const {
		// Assign the sampler texture unit
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		for (unsigned int i = 0; i < m_Textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			std::stringstream ss;
			std::string number;
			std::string name = m_Textures[i].type;
			if (name == "texture_diffuse")
				ss << diffuseNr++;
			else if (name == "texture_specular")
				ss << specularNr++;
			number = ss.str();

			shader.setUniform1i(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
		}

		// Draw
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

} }