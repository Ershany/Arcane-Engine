#include "DynamicLightManager.h"

namespace arcane { namespace graphics {

	// TODO: Add functionality so it can update with an entity's position and orientation
	DynamicLightManager::DynamicLightManager() 
		: m_DirectionalLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
		  m_SpotLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			          glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f) 
	{
		init();
	}

	void DynamicLightManager::init() {
		// Setup lighting configurations
		m_DirectionalLight.isActive = true;
		m_DirectionalLight.direction = glm::vec3(-0.1f, -1.0f, -0.1f);
		m_DirectionalLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		m_DirectionalLight.diffuse = glm::vec3(1.25f, 1.25f, 1.25f);
		m_DirectionalLight.specular = glm::vec3(1.5f, 1.5f, 1.5f);

		m_SpotLight.isActive = true;
		m_SpotLight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		m_SpotLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		m_SpotLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
		m_SpotLight.position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_SpotLight.direction = glm::vec3(1.0f, 0.0f, 0.0f);
		m_SpotLight.constant = 1.0f;
		m_SpotLight.linear = 0.022f;
		m_SpotLight.quadratic = 0.0019f;
		m_SpotLight.cutOff = glm::cos(glm::radians(12.5f));
		m_SpotLight.outerCutOff = glm::cos(glm::radians(15.0f));

		PointLight pointLight1(glm::vec3(0.05f, 0.05f, 0.05f), 
			glm::vec3(1.0f, 1.0f, 1.0f), 
			glm::vec3(1.0f, 1.0f, 1.0f), 
			glm::vec3(225.0f, 54.0f, 98.0f),
			1.0f, 
			0.007f, 
			0.0002f);
		pointLight1.isActive = true;
		addPointLight(pointLight1);
		/*

		PointLight pointLight2(glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(195.0f, 54.0f, 98.0f),
			1.0f,
			0.007f,
			0.0002f);
		pointLight2.isActive = true;
		addPointLight(pointLight2);

		PointLight pointLight3(glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(200.0f, 54.0f, 98.0f),
			1.0f,
			0.007f,
			0.0002f);
		pointLight3.isActive = true;
		addPointLight(pointLight3);

		PointLight pointLight4(glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(195.0f, 54.0f, 101.0f),
			1.0f,
			0.007f,
			0.0002f);
		pointLight4.isActive = true;
		addPointLight(pointLight4);

		PointLight pointLight5(glm::vec3(0.05f, 0.05f, 0.05f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(195.0f, 54.0f, 95.0f),
			1.0f,
			0.007f,
			0.0002f);
		pointLight5.isActive = true;
		addPointLight(pointLight5);
		*/
	}

	// TODO: Dynamically change the size of the lights or use only the close lights (LIMIT OF 5 CURRENTLY FOR POINTLIGHTS)
	void DynamicLightManager::setupLightingUniforms(Shader &shader) {
		shader.setUniform1i("numPointLights", m_PointLights.size());

		m_DirectionalLight.setupUniforms(shader, 0);
		m_SpotLight.setupUniforms(shader, 0);
		int i = 0;
		for (auto iter = m_PointLights.begin(); iter != m_PointLights.end(); iter++, i++) {
			(*iter).setupUniforms(shader, i);
		}
	}

	void DynamicLightManager::addPointLight(PointLight &pointLight) {
		m_PointLights.push_back(pointLight);
	}

} }