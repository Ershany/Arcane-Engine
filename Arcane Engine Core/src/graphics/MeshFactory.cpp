#include "MeshFactory.h"

namespace arcane { namespace graphics {

	Mesh* MeshFactory::CreateQuad(const char *path, bool shouldHaveSpec) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		vertices.push_back(Vertex(glm::vec3(-1, 0, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)));
		vertices.push_back(Vertex(glm::vec3(1, 0, 1), glm::vec3(0, 1, 0), glm::vec2(1, 1)));
		vertices.push_back(Vertex(glm::vec3(-1, 0, -1), glm::vec3(0, 1, 0), glm::vec2(0, 0)));
		vertices.push_back(Vertex(glm::vec3(1, 0, -1), glm::vec3(0, 1, 0), glm::vec2(1, 0)));

		// Load indices
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(2); indices.push_back(1); indices.push_back(3);

		// Load textures
		Texture texture;
		texture.id = opengl::Utility::loadTextureFromFile(path, true); // Specifies that are texture may contain transparency
		texture.type = "texture_diffuse";
		textures.push_back(texture);

		if (shouldHaveSpec) {
			texture.id = opengl::Utility::loadTextureFromFile("res/textures/fullSpec.png");
		}
		else {
			texture.id = opengl::Utility::loadTextureFromFile("res/textures/noSpec.png");
		}
		texture.type = "texture_specular";
		textures.push_back(texture);


		return new Mesh(vertices, indices, textures);
	}

} }