#pragma once

#include <utils/FileUtils.h>

namespace arcane {

	class Shader {
	private:
		unsigned int m_ShaderID;
		const char *m_VertPath, *m_FragPath, *m_GeomPath, *m_HullShader, *m_DomainShader;
	public:
		Shader(const char *vertPath, const char *fragPath);
		Shader(const char *vertPath, const char *fragPath, const char *geoPath);
		Shader(const char *vertPath, const char *fragPath, const char *hullPath, const char *domainPath);
		Shader(const char *vertPath, const char *fragPath, const char *geoPath, const char *hullPath, const char *domainPath);
		~Shader();

		void enable() const;
		void disable() const;

		void setUniform(const char *name, float value);
		void setUniform(const char *name, int value);
		void setUniform(const char *name, const glm::vec2& vector);
		void setUniform(const char *name, const glm::ivec2& vector);
		void setUniform(const char *name, const glm::vec3& vector);
		void setUniform(const char *name, const glm::ivec3& vector);
		void setUniform(const char *name, const glm::vec4& vector);
		void setUniform(const char *name, const glm::ivec4& vector);
		void setUniform(const char *name, const glm::mat3& matrix);
		void setUniform(const char *name, const glm::mat4& matrix);

		void setUniformArray(const char *name, int arraySize, float *value);
		void setUniformArray(const char *name, int arraySize, int *value);
		void setUniformArray(const char *name, int arraySize, glm::vec2 *value);
		void setUniformArray(const char *name, int arraySize, glm::ivec2 *value);
		void setUniformArray(const char *name, int arraySize, glm::vec3 *value);
		void setUniformArray(const char *name, int arraySize, glm::ivec3 *value);
		void setUniformArray(const char *name, int arraySize, glm::vec4 *value);
		void setUniformArray(const char *name, int arraySize, glm::ivec4 *value);

		inline unsigned int getShaderID() { return m_ShaderID; }
	private:
		int getUniformLocation(const char *name);

		unsigned int load();
	};

}
