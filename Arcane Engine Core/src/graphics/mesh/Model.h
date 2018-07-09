#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Shader.h"
#include "Mesh.h"
#include "NewMesh.h"

namespace arcane { namespace graphics {

	class Model {
	public:
		Model(const char *path);
		Model(const std::vector<NewMesh> &meshes);
		
		void Draw() const;
	private:
		static std::vector<Texture> m_LoadedTextures; // Used so the same texture doesn't get loaded into memory twice
		std::vector<NewMesh> m_Meshes;
		std::string m_Directory;


		void loadModel(const std::string &path);
		void processNode(aiNode *node, const aiScene *scene);
		NewMesh processMesh(aiMesh *mesh, const aiScene *scene);
		Texture* loadMaterialTexture(aiMaterial *mat, aiTextureType type, const char *typeName);
	};

} }