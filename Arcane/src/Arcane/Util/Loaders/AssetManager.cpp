#include "arcpch.h"
#include "AssetManager.h"

#include <Arcane/Graphics/Texture/Cubemap.h>
#include <Arcane/Graphics/Mesh/Model.h>

namespace Arcane
{
	AssetManager::AssetManager() : m_AsyncLoadingActive(true)
	{
		// Query for how many cores are on the machine
		int threadCount = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() / 2: 2;
		ARC_LOG_INFO("Spawning {0} threads for the asset manager", threadCount);
		for (int i = 0; i < threadCount; i++)
		{
			m_WorkerThreads.push_back(std::thread(&AssetManager::LoaderThread, this));
		}
	}

	AssetManager::~AssetManager()
	{
		for (int i = 0; i < m_WorkerThreads.size(); i++)
		{
			m_WorkerThreads[i].join();
		}
	}

	AssetManager& AssetManager::GetInstance()
	{
		static AssetManager manager;
		return manager;
	}


	Model* AssetManager::LoadModel(const std::string &path)
	{
		// Check the cache
		Model *modelCached = FetchModelFromCache(path);
		if (modelCached)
			return modelCached;

		Model *model = new Model();

		model->LoadModel(path);
		model->GenerateGpuData();

		m_ModelCache.insert(std::pair<std::string, Model*>(path, model));

		return model;
	}

	Model* AssetManager::LoadModelAsync(const std::string &path, std::function<void(Model*)> callback)
	{
		// Check the cache
		Model *modelCached = FetchModelFromCache(path);
		if (modelCached)
			return modelCached;

		Model *model = new Model();

		ModelLoadJob job;
		job.path = path;
		job.model = model;
		if (callback)
			job.callback = callback;
		m_ModelCache.insert(std::pair<std::string, Model*>(path, model));
		
		++m_AssetsInFlight;
		m_LoadingModelQueue.Push(job);

		return model;
	}

	Model* AssetManager::FetchModelFromCache(const std::string &path)
	{
		auto iter = m_ModelCache.find(path);
		if (iter != m_ModelCache.end())
		{
			return iter->second;
		}
		
		return nullptr;
	}

	// Function force loads a texture on the main thread and blocks until it is generated
	Texture* AssetManager::Load2DTexture(const std::string &path, TextureSettings *settings)
	{
		// Check the cache
		Texture *textureCached = FetchTextureFromCache(path);
		if (textureCached)
			return textureCached;

		Texture *texture;
		if (settings != nullptr)
		{
			texture = new Texture(*settings);
		}
		else
		{
			texture = new Texture();
		}

		TextureGenerationData genData;
		genData.texture = texture;

		TextureLoader::Load2DTextureData(path, genData);
		if (!genData.data)
		{
			delete texture;
			return nullptr;
		}

		TextureLoader::Generate2DTexture(path, genData);

		m_TextureCache.insert(std::pair<std::string, Texture*>(path, texture));

		return texture;
	}

	// Function adds the texture to a queue to be loaded by the asset manager's workers threads
	Texture* AssetManager::Load2DTextureAsync(const std::string &path, TextureSettings *settings, std::function<void(Texture*)> callback)
	{
		// Check the cache
		Texture *textureCached = FetchTextureFromCache(path);
		if (textureCached)
			return textureCached;

		Texture *texture;
		if (settings != nullptr)
		{
			texture = new Texture(*settings);
		}
		else
		{
			texture = new Texture();
		}

		// Create the job for the worker threads, but before adding it to the queue, add it to the texture cache
		TextureLoadJob job;
		job.texturePath = path;
		job.generationData.texture = texture;
		if (callback)
			job.callback = callback;
		m_TextureCache.insert(std::pair<std::string, Texture*>(path, texture));

		++m_AssetsInFlight;
		m_LoadingTexturesQueue.Push(job);

		return texture;
	}

	Texture* AssetManager::FetchTextureFromCache(const std::string &path)
	{
		auto iter = m_TextureCache.find(path);
		if (iter != m_TextureCache.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	Cubemap* AssetManager::LoadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings)
	{
		Cubemap *cubemap = new Cubemap();
		if (settings != nullptr)
			cubemap->SetCubemapSettings(*settings);

		std::vector<std::string> faces = { right, left, top, bottom, back, front };
		for (int i = 0; i < faces.size(); i++)
		{
			CubemapGenerationData genData;
			genData.cubemap = cubemap;
			genData.face = (GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			TextureLoader::LoadCubemapTextureData(faces[i], genData);
			if (!genData.data)
			{
				delete cubemap;
				return nullptr;
			}

			TextureLoader::GenerateCubemapTexture(faces[i], genData);
		}

		return cubemap;
	}

	Cubemap* AssetManager::LoadCubemapTextureAsync(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings, std::function<void()> callback)
	{
		Cubemap *cubemap = new Cubemap();
		if (settings != nullptr)
			cubemap->SetCubemapSettings(*settings);

		std::vector<std::string> faces = { right, left, top, bottom, back, front };
		for (int i = 0; i < faces.size(); i++)
		{
			CubemapLoadJob job;
			job.texturePath = faces[i];
			job.generationData.face = (GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			job.generationData.cubemap = cubemap;
			if (callback)
				job.callback = callback;

			++m_AssetsInFlight;
			m_LoadingCubemapQueue.Push(job);
		}

		return cubemap;
	}

	void AssetManager::LoaderThread()
	{
		while (m_AsyncLoadingActive)
		{
			if (!m_LoadingTexturesQueue.Empty())
			{
				TextureLoadJob loadJob;
				if (m_LoadingTexturesQueue.TryPop(loadJob))
				{
					TextureLoader::Load2DTextureData(loadJob.texturePath, loadJob.generationData);
					m_GenerateTexturesQueue.Push(loadJob);
				}
			}
			if (!m_LoadingCubemapQueue.Empty())
			{
				CubemapLoadJob loadJob;
				if (m_LoadingCubemapQueue.TryPop(loadJob))
				{
					TextureLoader::LoadCubemapTextureData(loadJob.texturePath, loadJob.generationData);
					m_GenerateCubemapQueue.Push(loadJob);
				}
			}
			if (!m_LoadingModelQueue.Empty())
			{
				ModelLoadJob loadJob;
				if (m_LoadingModelQueue.TryPop(loadJob))
				{
					loadJob.model->LoadModel(loadJob.path);
					m_GenerateModelQueue.Push(loadJob);
				}
			}

			// Should these asset threads be sleeping?
			unsigned int workLoadSizeRemaining = m_LoadingTexturesQueue.Size() + m_LoadingCubemapQueue.Size() + m_LoadingModelQueue.Size();
			const int msToWait = 10;
			if (workLoadSizeRemaining <= 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(msToWait));
		}
	}

	void AssetManager::Update(int texturesPerFrame, int cubemapFacesPerFrame, int modelsPerFrame)
	{
		// Must be done on the main thread since OpenGL is single-threaded in nature
		while (!m_GenerateTexturesQueue.Empty())
		{
			TextureLoadJob loadJob;
			if (m_GenerateTexturesQueue.TryPop(loadJob))
			{
				if (!loadJob.generationData.data)
				{
					m_TextureCache.erase(loadJob.texturePath);
					delete loadJob.generationData.texture;
					--m_AssetsInFlight;
					break;
				}

				TextureLoader::Generate2DTexture(loadJob.texturePath, loadJob.generationData);
				--m_AssetsInFlight;
				if (loadJob.callback)
					loadJob.callback(loadJob.generationData.texture);

				if (--texturesPerFrame <= 0)
					break;
			}
		}
		while (!m_GenerateCubemapQueue.Empty())
		{
			CubemapLoadJob loadJob;
			if (m_GenerateCubemapQueue.TryPop(loadJob))
			{
				if (!loadJob.generationData.data)
				{
					delete loadJob.generationData.data;
					--m_AssetsInFlight;
					break;
				}

				TextureLoader::GenerateCubemapTexture(loadJob.texturePath, loadJob.generationData);
				--m_AssetsInFlight;
				if (loadJob.callback)
					loadJob.callback();

				if (--cubemapFacesPerFrame <= 0)
					break;
			}
		}
		while (!m_GenerateModelQueue.Empty())
		{
			ModelLoadJob loadJob;
			if (m_GenerateModelQueue.TryPop(loadJob))
			{
				if (loadJob.model->m_Meshes.size() == 0)
				{
					m_ModelCache.erase(loadJob.path);
					delete loadJob.model;
					--m_AssetsInFlight;
					break;
				}

				loadJob.model->GenerateGpuData();
				--m_AssetsInFlight;
				if (loadJob.callback)
					loadJob.callback(loadJob.model);

				if (--modelsPerFrame <= 0)
					break;
			}
		}
	}
}
