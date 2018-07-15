#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Shader.h"
#include "Mesh.h"

namespace arcane { namespace graphics {

	struct Texture {
		unsigned int id;
		std::string type;
		aiString path; // Allows us to compare with other textures so no duplicate textures are generated. TODO: change to a proper texture loading system
	};

	class Model {
	public:
		Model(const char *path);
		Model(const std::vector<Mesh> &meshes);
		
		void Draw(Shader &shader) const;
	private:
		static std::vector<Texture> m_LoadedTextures; // Used so the same texture doesn't get loaded into memory twice
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;

		void loadModel(const std::string &path);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		unsigned int loadMaterialTexture(aiMaterial *mat, aiTextureType type, const char *typeName);
	};

} }