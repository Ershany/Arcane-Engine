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

		void setUniform1f(const char *name, float value);
		void setUniform1i(const char *name, int value);
		void setUniform2f(const char *name, const glm::vec2& vector);
		void setUniform2i(const char *name, const glm::ivec2& vector);
		void setUniform3f(const char *name, const glm::vec3& vector);
		void setUniform3i(const char *name, const glm::ivec3& vector);
		void setUniform4f(const char *name, const glm::vec4& vector);
		void setUniform4i(const char *name, const glm::ivec4& vector);
		void setUniformMat3(const char *name, const glm::mat3& matrix);
		void setUniformMat4(const char *name, const glm::mat4& matrix);

		void setUniform1fv(const char *name, int arraySize, float *value);
		void setUniform1iv(const char *name, int arraySize, int *value);
		void setUniform2fv(const char *name, int arraySize, glm::vec2 *value);
		void setUniform2iv(const char *name, int arraySize, glm::ivec2 *value);
		void setUniform3fv(const char *name, int arraySize, glm::vec3 *value);
		void setUniform3iv(const char *name, int arraySize, glm::ivec3 *value);
		void setUniform4fv(const char *name, int arraySize, glm::vec4 *value);
		void setUniform4iv(const char *name, int arraySize, glm::ivec4 *value);

		inline unsigned int getShaderID() { return m_ShaderID; }
	private:
		int getUniformLocation(const char *name);

		unsigned int load();
	};

}
