#pragma once

#include <graphics/Shader.h>
#include <graphics/mesh/Mesh.h>
#include <graphics/renderer/renderpass/RenderPassType.h>
#include <utils/loaders/TextureLoader.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Arcane
{
	class Model {
	public:
		Model(const char *path);
		Model(const Mesh &mesh);
		Model(const std::vector<Mesh> &meshes);
		
		void Draw(Shader *shader, RenderPassType pass) const;

		inline std::vector<Mesh>& GetMeshes() { return m_Meshes; }
	private:
		void LoadModel(const std::string &path);
		void ProcessNode(aiNode *node, const aiScene *scene);
		Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
		Texture* LoadMaterialTexture(aiMaterial *mat, aiTextureType type, bool isSRGB);
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;
	};
}
