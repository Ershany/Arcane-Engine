#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

namespace arcane { namespace graphics {

	class Model {
	public:
		Model(const char *path);
		
		void Draw(Shader &shader) const;
	private:
		std::vector<Texture> m_LoadedTextures; // Used so the same texture doesn't get loaded into memory twice
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;


		void loadModel(const std::string &path);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
		unsigned int TextureFromFile(const char *path, const std::string &directory);
	};

} }