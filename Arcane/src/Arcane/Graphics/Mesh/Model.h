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
		friend class AssetManager;
	public:
		Model();
		Model(const Mesh &mesh);
		Model(const std::vector<Mesh> &meshes);
		
		void Draw(Shader *shader, RenderPassType pass) const;

		inline std::vector<Mesh>& GetMeshes() { return m_Meshes; }

		inline const std::string& GetName() const { return m_Name; }
		inline std::string& GetNameRef() { return m_Name; }

		inline auto* GetBoneDataMap() { return &m_BoneDataMap; }
		inline int& GetBoneCountRef() { return m_BoneCount; }

		static inline glm::mat4 ConvertAssimpMatrixToGLM(const aiMatrix4x4& aiMat)
		{
			return glm::transpose(glm::make_mat4(&aiMat.a1));
		}
	private:
		void LoadModel(const std::string &path);
		void GenerateGpuData();

		void ProcessNode(aiNode *node, const aiScene *scene);
		void ProcessMesh(aiMesh *mesh, const aiScene *scene);
		Texture* LoadMaterialTexture(aiMaterial *mat, aiTextureType type, bool isSRGB);
	private:
		std::vector<Mesh> m_Meshes;
		std::unordered_map<std::string, BoneData> m_BoneDataMap;
		int m_BoneCount;

		std::string m_Directory;
		std::string m_Name;
	};
}
