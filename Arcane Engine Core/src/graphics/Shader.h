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
		void setUniform3f(const char *name, const glm::vec3& vector);
		void setUniform4f(const char *name, const glm::vec4& vector);
		void setUniform4i(const char *name, const glm::ivec4& vector);
		void setUniformMat3(const char *name, const glm::mat3& matrix);
		void setUniformMat4(const char *name, const glm::mat4& matrix);

		inline unsigned int getShaderID() { return m_ShaderID; }
	private:
		int getUniformLocation(const char *name);

		unsigned int load();
	};

}
