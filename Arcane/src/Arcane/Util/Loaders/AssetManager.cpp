#include "arcpch.h"
#include "AssetManager.h"

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

			// Should these asset threads be sleeping?
			// unsigned int workLoadSizeRemaining = m_LoadingTexturesQueue.Size();
			// const int msToWait = workLoadSizeRemaining > 0 ? 10 : 50;
			//std::this_thread::sleep_for(std::chrono::milliseconds(msToWait));
		}
	}

	void AssetManager::Update(int texturesPerFrame)
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
	}
}
