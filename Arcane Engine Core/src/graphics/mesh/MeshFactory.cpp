#include "MeshFactory.h"

namespace arcane { namespace graphics {

	Mesh* MeshFactory::CreateQuad(const char *path, bool shouldHaveSpec) {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		positions.push_back(glm::vec3(-1, 0, 1)); uvs.push_back(glm::vec2(0, 1)); normals.push_back(glm::vec3(0, 1, 0));
		positions.push_back(glm::vec3(1, 0, 1)); uvs.push_back(glm::vec2(1, 1)); normals.push_back(glm::vec3(0, 1, 0));
		positions.push_back(glm::vec3(-1, 0, -1)); uvs.push_back(glm::vec2(0, 0)); normals.push_back(glm::vec3(0, 1, 0));
		positions.push_back(glm::vec3(1, 0, -1)); uvs.push_back(glm::vec2(1, 0)); normals.push_back(glm::vec3(0, 1, 0));

		// Load indices
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(2); indices.push_back(1); indices.push_back(3);

		Mesh *mesh = new Mesh(positions, uvs, normals, indices);
		mesh->LoadData();

		// Finally create the material
		mesh->getMaterial().setDiffuseMapId(opengl::Utility::loadTextureFromFile(path, true));
		if (shouldHaveSpec) {
			mesh->getMaterial().setSpecularMapId(opengl::Utility::loadTextureFromFile("res/textures/fullSpec.png"));
		}
		else {
			mesh->getMaterial().setSpecularMapId(opengl::Utility::loadTextureFromFile("res/textures/noSpec.png"));
		}

		return mesh;
	}
	
	Mesh* MeshFactory::CreateScreenQuad(int colourBufferId) {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		positions.push_back(glm::vec3(-1, 1, 0)); uvs.push_back(glm::vec2(0, 1)); normals.push_back(glm::vec3(0, 0, 1));
		positions.push_back(glm::vec3(1, 1, 0)); uvs.push_back(glm::vec2(1, 1)); normals.push_back(glm::vec3(0, 0, 1));
		positions.push_back(glm::vec3(-1, -1, 0)); uvs.push_back(glm::vec2(0, 0)); normals.push_back(glm::vec3(0, 0, 1));
		positions.push_back(glm::vec3(1, -1, 0)); uvs.push_back(glm::vec2(1, 0)); normals.push_back(glm::vec3(0, 0, 1));

		// Load indices
		indices.push_back(1); indices.push_back(0); indices.push_back(2);
		indices.push_back(3); indices.push_back(1); indices.push_back(2);

		Mesh *mesh = new Mesh(positions, uvs, normals, indices);
		mesh->LoadData();

		// Finally create the material
		mesh->getMaterial().setDiffuseMapId(colourBufferId);

		return mesh;
	}

} }