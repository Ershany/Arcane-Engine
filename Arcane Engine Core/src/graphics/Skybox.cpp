#include "pch.h"
#include "Skybox.h"

#include <utils/loaders/ShaderLoader.h>

namespace arcane {

	Skybox::Skybox(const std::vector<std::string> &filePaths) {
		m_SkyboxShader = ShaderLoader::loadShader("src/shaders/skybox.vert", "src/shaders/skybox.frag");

		m_SkyboxCubemap = TextureLoader::loadCubemapTexture(filePaths[0], filePaths[1], filePaths[2], filePaths[3], filePaths[4], filePaths[5], true);
		
		float skyboxVertices[] = {
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
		unsigned int skyboxIndices[] = {
			// front
			2, 1, 0,
			0, 3, 2,
			// top
			6, 5, 1,
			1, 2, 6,
			// back
			5, 6, 7,
			7, 4, 5,
			// bottom
			3, 0, 4,
			4, 7, 3,
			// left
			1, 5, 4,
			4, 0, 1,
			// right
			6, 2, 3,
			3, 7, 6,
		};

		m_SkyboxVBO.load(skyboxVertices, 8 * 3, 3); 
		m_SkyboxIBO.load(skyboxIndices, 36);
		m_SkyboxVAO.addBuffer(&m_SkyboxVBO, 0);

		m_GLCache = GLCache::getInstance();
	}

	void Skybox::Draw(ICamera *camera) {
		m_GLCache->switchShader(m_SkyboxShader);

		// Pass the texture to the shader
		m_SkyboxCubemap->bind(0);
		m_SkyboxShader->setUniform1i("skyboxCubemap", 0);

		m_SkyboxShader->setUniformMat4("view", camera->getViewMatrix());
		m_SkyboxShader->setUniformMat4("projection", camera->getProjectionMatrix());

		// Since the vertex shader is gonna make the depth value 1.0, and the default value in the depth buffer is 1.0 so this is needed to draw the sky  box
		m_GLCache->setDepthFunc(GL_LEQUAL);
		m_SkyboxVAO.bind();
		m_SkyboxIBO.bind();
		glDrawElements(GL_TRIANGLES, m_SkyboxIBO.getCount(), GL_UNSIGNED_INT, 0);
		m_SkyboxVAO.unbind();
		m_SkyboxIBO.unbind();
		m_GLCache->setDepthFunc(GL_LESS);
	}

}
