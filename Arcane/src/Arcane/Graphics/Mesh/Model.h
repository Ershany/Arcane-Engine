#pragma once

#include <Arcane/Graphics/Mesh/Mesh.h>
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Arcane
{
	class Shader;

	class Model {
	public:
		Model(const char *path);
		Model(const Mesh &mesh);
		Model(const std::vector<Mesh> &meshes);
		
		void Draw(Shader *shader, RenderPassType pass) const;

		inline std::vector<Mesh>& GetMeshes() { return m_Meshes; }

		inline const std::string& GetName() const { return m_Name; }
	private:
		void LoadModel(const std::string &path);
		void ProcessNode(aiNode *node, const aiScene *scene);
		Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
		Texture* LoadMaterialTexture(aiMaterial *mat, aiTextureType type, bool isSRGB);
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;
		std::string m_Name;
	};
}
