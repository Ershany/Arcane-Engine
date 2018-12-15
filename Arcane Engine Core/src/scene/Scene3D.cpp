#include "pch.h"
#include "Scene3D.h"

#include <graphics/mesh/Mesh.h>
#include <graphics/mesh/common/Cube.h>
#include <graphics/mesh/common/Sphere.h>
#include <graphics/mesh/common/Quad.h>

namespace arcane {

	Scene3D::Scene3D(FPSCamera *camera, Window *window)
		: m_TerrainShader("src/shaders/terrain.vert", "src/shaders/terrain.frag"), m_ModelShader("src/shaders/pbr_model.vert", "src/shaders/pbr_model.frag"), m_Camera(camera),
		  m_ShadowmapShader("src/shaders/shadowmap.vert", "src/shaders/shadowmap.frag"), m_DynamicLightManager()
	{
		m_MeshRenderer = new MeshRenderer(camera);
		m_GLCache = GLCache::getInstance();
		m_Terrain = new Terrain(glm::vec3(0.0f, -20.0f, 0.0f));

		init();
	}

	Scene3D::~Scene3D() {

	}

	void Scene3D::init() {
		m_GLCache->setMultisample(true);

		// Load renderables
		/*
		Quad windowPane;
		windowPane.getMaterial().setDiffuseMap(TextureLoader::load2DTexture(std::string("res/textures/window.png"), true));
		windowPane.getMaterial().setSpecularMap(TextureLoader::load2DTexture(std::string("res/textures/default/fullSpec.png"), false));
		Model *glass = new Model(windowPane);
		*/

		//add(new SceneNode(glm::vec3(200.0f, 50.0f, 100.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(0.0f), new arcane::Model("res/3D_Models/Sponza/sponza.obj"), nullptr, false));
		
		//add(new SceneNode(glm::vec3(40, 60, 40), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, true));
		//add(new SceneNode(glm::vec3(80, 60, 80), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, true));
		//add(new SceneNode(glm::vec3(120, 60, 120), glm::vec3(15, 15, 15), glm::vec3(0.0, 1.0, 0.0), glm::radians(180.0f), glass, nullptr, true));

		//add(new SceneNode(glm::vec3(20, 90, 20), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new Model(Cube()), nullptr, false));
		//add(new SceneNode(glm::vec3(140, 90, 140), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new Model(Sphere()), nullptr, false));
		//add(new SceneNode(glm::vec3(-20, 90, -20), glm::vec3(10, 10, 10), glm::vec3(1, 0, 0), 0, new Model(Quad()), nullptr, false));

		// Temp code until I rewrite the model loader
		Model *pbrGun = new arcane::Model("res/3D_Models/Cerberus_Gun/Cerberus_LP.FBX");
		add(new Renderable3D(glm::vec3(120.0f, 75.0f, 120.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(-90.0f), pbrGun, nullptr, false));
		pbrGun->getMeshes()[0].getMaterial().setAlbedoMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_A.tga"), true));
		pbrGun->getMeshes()[0].getMaterial().setNormalMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_N.tga"), false));
		pbrGun->getMeshes()[0].getMaterial().setMetallicMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_M.tga"), false));
		pbrGun->getMeshes()[0].getMaterial().setRoughnessMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_R.tga"), false));
		pbrGun->getMeshes()[0].getMaterial().setAmbientOcclusionMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Cerberus_Gun/Textures/Cerberus_AO.tga"), false));

		// Temp testing code
		/*
		int nrRows = 2;
		int nrColumns = 2;
		float spacing = 2.5;
		for (int row = 0; row < nrRows; row++) {
			for (int col = 0; col < nrColumns; col++) {
				Model *sphere = new arcane::Model("res/3D_Models/Sphere/globe-sphere.obj");
				Material &mat = sphere->getMeshes()[0].getMaterial();
				mat.setAlbedoMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Sphere/rustediron2_basecolor.png"), true));
				mat.setNormalMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Sphere/rustediron2_normal.png"), false));
				mat.setAmbientOcclusionMap(TextureLoader::load2DTexture(std::string("res/textures/default/white.png"), false));
				mat.setMetallicMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Sphere/rustediron2_metallic.png"), false));
				mat.setRoughnessMap(TextureLoader::load2DTexture(std::string("res/3D_Models/Sphere/rustediron2_roughness.png"), false));
				add(new SceneNode(glm::vec3((float)(col - (nrColumns / 2)) * spacing,
					(float)(row - (nrRows / 2)) * spacing, 0.0f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f, sphere, nullptr, false));
			}
		}
		*/

		// Skybox
		std::vector<std::string> skyboxFilePaths;
		skyboxFilePaths.push_back("res/skybox/right.png");
		skyboxFilePaths.push_back("res/skybox/left.png");
		skyboxFilePaths.push_back("res/skybox/top.png");
		skyboxFilePaths.push_back("res/skybox/bottom.png");
		skyboxFilePaths.push_back("res/skybox/back.png");
		skyboxFilePaths.push_back("res/skybox/front.png");
		m_Skybox = new Skybox(skyboxFilePaths, m_Camera);
	}

	void Scene3D::shadowmapPass() {
		// Temporary location code for the shadowmap. Will move to a proper system with CSM (Cascaded shadow maps)
		glm::vec3 dirLightShadowmapLookAtPos = m_Camera->getPosition() + (glm::normalize(glm::vec3(m_Camera->getFront().x, 0.0f, m_Camera->getFront().z)) * 50.0f);
		glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos + (-m_DynamicLightManager.getDirectionalLightDirection() * 100.0f);

		m_GLCache->switchShader(m_ShadowmapShader.getShaderID());
		glm::mat4 directionalLightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, SHADOWMAP_NEAR_PLANE, SHADOWMAP_FAR_PLANE);
		glm::mat4 directionalLightView = glm::lookAt(dirLightShadowmapEyePos, dirLightShadowmapLookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 directionalLightViewProjMatrix = directionalLightProjection * directionalLightView;
		m_ShadowmapShader.setUniformMat4("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

		addObjectsToRenderQueue();

		m_MeshRenderer->flushOpaque(m_ShadowmapShader, RenderPass::ShadowmapPass);
		m_MeshRenderer->flushTransparent(m_ShadowmapShader, RenderPass::ShadowmapPass);
		m_Terrain->Draw(m_ShadowmapShader, RenderPass::ShadowmapPass);

		// Setup shadow uniforms for other shaders so they can have normal mapping
		m_GLCache->switchShader(m_TerrainShader.getShaderID());
		m_TerrainShader.setUniformMat4("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

		m_GLCache->switchShader(m_ModelShader.getShaderID());
		m_ModelShader.setUniformMat4("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);
	}

	float rotAmount = 0.0f;
	void Scene3D::onUpdate(float deltaTime) {
		//m_Renderables[0]->setOrientation(rotAmount, glm::vec3(0.0f, 1.0f, 0.0f));
		rotAmount += deltaTime;
	}

	void Scene3D::onRender(unsigned int shadowmap) {
		// Setup
		glm::mat4 projectionMatrix = m_Camera->getProjectionMatrix();
		m_DynamicLightManager.setSpotLightDirection(m_Camera->getFront());
		m_DynamicLightManager.setSpotLightPosition(m_Camera->getPosition());

		// Models
		m_GLCache->switchShader(m_ModelShader.getShaderID());
		m_DynamicLightManager.setupLightingUniforms(m_ModelShader);
		m_ModelShader.setUniform3f("viewPos", m_Camera->getPosition());
		m_ModelShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_ModelShader.setUniformMat4("projection", projectionMatrix);

		// Shadow map code
		m_ModelShader.setUniform1i("shadowmap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmap);

		// IBL code
		m_ModelShader.setUniform1i("irradianceMap", 1);
		m_Skybox->getSkyboxCubemap()->bind(1);

		// Add objects to the renderer
		addObjectsToRenderQueue();

		// Opaque objects
		m_MeshRenderer->flushOpaque(m_ModelShader, RenderPass::LightingPass);

		// Terrain
		m_GLCache->switchShader(m_TerrainShader.getShaderID());

		m_TerrainShader.setUniform1i("shadowmap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowmap);

		m_DynamicLightManager.setupLightingUniforms(m_TerrainShader);
		m_TerrainShader.setUniform3f("viewPos", m_Camera->getPosition());
		glm::mat4 modelMatrix(1);
		modelMatrix = glm::translate(modelMatrix, m_Terrain->getPosition());
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
		m_TerrainShader.setUniformMat3("normalMatrix", normalMatrix);
		m_TerrainShader.setUniformMat4("model", modelMatrix);
		m_TerrainShader.setUniformMat4("view", m_Camera->getViewMatrix());
		m_TerrainShader.setUniformMat4("projection", projectionMatrix);
		m_Terrain->Draw(m_TerrainShader, RenderPass::LightingPass);

		// Skybox
		m_Skybox->Draw();

		// Transparent objects
		m_GLCache->switchShader(m_ModelShader.getShaderID());
		m_MeshRenderer->flushTransparent(m_ModelShader, RenderPass::LightingPass);
	}

	void Scene3D::add(Renderable3D *renderable) {
		m_Renderables.push_back(renderable);
	}

	void Scene3D::addObjectsToRenderQueue() {
		auto iter = m_Renderables.begin();
		while (iter != m_Renderables.end()) {
			Renderable3D *curr = *iter;
			if (curr->getTransparent()) {
				m_MeshRenderer->submitTransparent(curr);
			}
			else {
				m_MeshRenderer->submitOpaque(curr);
			}

			iter++;
		}
	}

}
