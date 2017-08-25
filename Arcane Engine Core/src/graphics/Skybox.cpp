#include "Skybox.h"

namespace arcane { namespace graphics {

	Skybox::Skybox(const std::vector<const char*> &filePaths, FPSCamera *camera, Window *window) : m_SkyboxShader("src/shaders/skybox.vert", "src/shaders/skybox.frag"), m_Camera(camera), m_Window(window)
	{
		m_SkyboxCubemap = opengl::Utility::loadCubemapFromFiles(filePaths);
		
		GLfloat skyboxVertices[] = {
			// Front
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// Back
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0,
		};
		GLuint skyboxIndices[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 0, 3,
			3, 7, 4,
			// left
			4, 5, 1,
			1, 0, 4,
			// right
			3, 2, 6,
			6, 7, 3,
		};

		// Is this correct?
		m_SkyboxVBO.load(skyboxVertices, 8 * 3, 3); 
		m_SkyboxIBO.load(skyboxIndices, 36);
		m_SkyboxVAO.addBuffer(&m_SkyboxVBO, 0);
	}

	void Skybox::Draw() {
		m_SkyboxShader.enable();

		// Pass the texture to the shader
		glActiveTexture(GL_TEXTURE0);
		m_SkyboxShader.setUniform1i("skyboxCubemap", 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxCubemap);

		m_SkyboxShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_SkyboxShader.setUniformMat4("projection", glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 1000.0f));

		glDisable(GL_DEPTH_TEST);
		m_SkyboxVAO.bind();
		m_SkyboxIBO.bind();
		glDrawElements(GL_TRIANGLES, m_SkyboxIBO.getCount(), GL_UNSIGNED_INT, 0);
		m_SkyboxVAO.unbind();
		m_SkyboxIBO.unbind();
		glEnable(GL_DEPTH_TEST);

		m_SkyboxShader.disable();
	}

} }