#pragma once

#include <graphics/Shader.h>
#include <graphics/mesh/Mesh.h>
#include <graphics/renderer/RenderPass.h>
#include <utils/loaders/TextureLoader.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace arcane { namespace graphics {

	class Model {
	public:
		Model(const char *path);
		Model(const Mesh &mesh);
		Model(const std::vector<Mesh> &meshes);
		
		void Draw(Shader &shader, RenderPass pass) const;

		inline std::vector<Mesh>& getMeshes() { return m_Meshes; }
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;

		void loadModel(const std::string &path);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		Texture* loadMaterialTexture(aiMaterial *mat, aiTextureType type, bool isSRGB);
	};

} }