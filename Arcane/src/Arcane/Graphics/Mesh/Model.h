#pragma once
#ifndef MODEL_H
#define MODEL_H

#ifndef MESH_H
#include <Arcane/Graphics/Mesh/Mesh.h>
#endif

#ifndef RENDERPASSTYPE_H
#include <Arcane/Graphics/Renderer/Renderpass/RenderPassType.h>
#endif

#include <assimp/material.h>
#include <assimp/matrix4x4.h>

struct aiScene;
struct aiMaterial;
struct aiNode;
struct aiMesh;

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

		inline const auto& GetGlobalInverseTransform() const { return m_GlobalInverseTransform; }

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
		glm::mat4 m_GlobalInverseTransform; // Used by animation for bone related data to move it back to the origin
		int m_BoneCount;

		std::string m_Directory;
		std::string m_Name;
	};
}
#endif
