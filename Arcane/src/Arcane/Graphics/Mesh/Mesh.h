#pragma once

#include <Arcane/Graphics/Mesh/Material.h>
#include <Arcane/Animation/AnimationData.h>

namespace Arcane
{
	class Mesh
	{
		friend class Model;
		friend class AssetManager;
	public:
		Mesh();
		Mesh(std::vector<glm::vec3>&& positions, std::vector<glm::vec2>&& uvs, std::vector<unsigned int>&& indices);
		Mesh(std::vector<glm::vec3>&& positions, std::vector<glm::vec2>&& uvs, std::vector<glm::vec3>&& normals, std::vector<glm::vec3>&& tangents, std::vector<glm::vec3>&& bitangents, std::vector<unsigned int>&& indices);
		Mesh(std::vector<glm::vec3> &&positions, std::vector<glm::vec2> &&uvs, std::vector<glm::vec3> &&normals, std::vector<glm::vec3> &&tangents, std::vector<glm::vec3> &&bitangents, std::vector<VertexBoneData> &&boneWeights, std::vector<unsigned int> &&indices);
		
		void LoadData(bool interleaved = true);
		void GenerateGpuData(); // Commits all of the buffers and their attributes to the GPU driver

		void Draw() const;

		inline Material& GetMaterial() { return m_Material; }
	protected:
		unsigned int m_VAO, m_VBO, m_IBO;
		Material m_Material;

		std::vector<glm::vec3> m_Positions;
		std::vector<glm::vec2> m_UVs;
		std::vector<glm::vec3> m_Normals;
		std::vector<glm::vec3> m_Tangents;
		std::vector<glm::vec3> m_Bitangents;
		std::vector<VertexBoneData> m_BoneData;

		std::vector<unsigned int> m_Indices;

		std::vector<float> m_BufferData;
		bool m_IsInterleaved;
		unsigned int m_BufferComponentCount;
	};
}
