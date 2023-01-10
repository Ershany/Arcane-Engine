#include "arcpch.h"
#include "AssetManager.h"

#include <Arcane/Graphics/Texture/Cubemap.h>

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

	bool AssetManager::TexturesInProgress()
	{
		return !m_LoadingTexturesQueue.Empty() || !m_GenerateTexturesQueue.Empty();
	}

	bool AssetManager::CubemapsInProgress()
	{
		return !m_LoadingCubemapQueue.Empty() || !m_GenerateCubemapQueue.Empty();
	}

	// Function force loads a texture on the main thread and blocks until it is generated
	Texture* AssetManager::Load2DTexture(std::string &path, TextureSettings *settings)
	{
		// Check the cache
		auto iter = m_TextureCache.find(path);
		if (iter != m_TextureCache.end())
		{
			return iter->second;
		}

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
	Texture* AssetManager::Load2DTextureAsync(std::string &path, TextureSettings *settings)
	{
		// Check the cache
		auto iter = m_TextureCache.find(path);
		if (iter != m_TextureCache.end())
		{
			return iter->second;
		}

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
		m_TextureCache.insert(std::pair<std::string, Texture*>(path, texture));

		m_LoadingTexturesQueue.Push(job);

		return texture;
	}

	Cubemap* AssetManager::LoadCubemapTexture(std::string &right, std::string &left, std::string &top, std::string &bottom, std::string &back, std::string &front, CubemapSettings *settings)
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

	Cubemap* AssetManager::LoadCubemapTextureAsync(std::string &right, std::string &left, std::string &top, std::string &bottom, std::string &back, std::string &front, CubemapSettings *settings)
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
					if (loadJob.generationData.data)
					{
						m_GenerateTexturesQueue.Push(loadJob);
					}
					else
					{
						delete loadJob.generationData.texture;
					}
				}
			}
			if (!m_LoadingCubemapQueue.Empty())
			{
				CubemapLoadJob loadJob;
				if (m_LoadingCubemapQueue.TryPop(loadJob))
				{
					TextureLoader::LoadCubemapTextureData(loadJob.texturePath, loadJob.generationData);
					if (loadJob.generationData.data)
					{
						m_GenerateCubemapQueue.Push(loadJob);
					}
					else
					{
						delete loadJob.generationData.data;
					}
				}
			}

			// Should these asset threads be sleeping?
			// unsigned int workLoadSizeRemaining = m_LoadingTexturesQueue.Size();
			// const int msToWait = workLoadSizeRemaining > 0 ? 10 : 50;
			//std::this_thread::sleep_for(std::chrono::milliseconds(msToWait));
		}
	}

	void AssetManager::Update(int texturesPerFrame, int cubemapFacesPerFrame)
	{
		// Must be done on the main thread since OpenGL is single-threaded in nature
		while (!m_GenerateTexturesQueue.Empty())
		{
			TextureLoadJob loadJob;
			if (m_GenerateTexturesQueue.TryPop(loadJob))
			{
				TextureLoader::Generate2DTexture(loadJob.texturePath, loadJob.generationData);

				if (--texturesPerFrame <= 0)
					break;
			}
		}
		while (!m_GenerateCubemapQueue.Empty())
		{
			CubemapLoadJob loadJob;
			if (m_GenerateCubemapQueue.TryPop(loadJob))
			{
				TextureLoader::GenerateCubemapTexture(loadJob.texturePath, loadJob.generationData);

				if (--cubemapFacesPerFrame <= 0)
					break;
			}
		}
	}
}
