#pragma once
#ifndef MESH_H
#define MESH_H

#ifndef MATERIAL_H
#include <Arcane/Graphics/Mesh/Material.h>
#endif

#ifndef ANIMATIONDATA_H
#include <Arcane/Animation/AnimationData.h>
#endif

namespace Arcane
{
	class Mesh
	{
		friend class Model;
		friend class AssetManager;

		// This works great for loading in different types of data into our vertex buffers. This will no longer be a valid strategy if we ever add a data type that isn't the same size
		// When that happens we should rework how we are loading in data anyways, since it will be a nice memory and speed optimization anyways. For now, this will do!
		union BufferData
		{
			float floatingPoint;
			int integer;
			BufferData(int i) : integer(i) {}
			BufferData(float f) : floatingPoint(f) {}
		};
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

		std::vector<BufferData> m_BufferData;
		bool m_IsInterleaved;
		unsigned int m_BufferComponentCount;
	};
}
#endif
