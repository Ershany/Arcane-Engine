#pragma once

#include <graphics/Shader.h>
#include <graphics/mesh/Mesh.h>
#include <graphics/mesh/Model.h>
#include <utils/loaders/TextureLoader.h>

namespace arcane {

	class Terrain {
	public:
		Terrain(glm::vec3 &worldPosition);
		~Terrain();

		void Draw(Shader *shader, RenderPassType pass) const;

		float sampleHeightfieldBilinear(const glm::vec3& worldPos);

		inline const glm::vec3& getPosition() const { return m_Position; }
		inline const std::vector<glm::vec3>& GetPoints() const { return m_Mesh->GetPositions(); }
		inline const unsigned int GetVertexCount() const { return m_VertexSideCount; }

	private:
		glm::vec3 calculateNormal(unsigned int x, unsigned int z, unsigned char *heightMapData);
		float getVertexHeight(unsigned int x, unsigned int y, unsigned char *heightMapData);
		float sampleHeightfieldNearest(const glm::vec3& worldPos);
		float clip(float n, float lower, float upper);

		float m_TerrainSize;
		unsigned int m_VertexSideCount;
		GLushort m_HeightMapScale;

		glm::mat4 m_ModelMatrix;
		glm::vec3 m_Position;
		Mesh *m_Mesh;
		std::array<Texture*, 9> m_Textures; // Represents all the textures supported by the terrain's texure splatting (rgba and the default value)
	};

}
