#pragma once

#include <Arcane/Core/Threads/ThreadSafeQueue.h>
#include <Arcane/Util/Loaders/TextureLoader.h>

namespace Arcane
{
	struct TextureSettings;

	struct TextureLoadJob
	{
		std::string texturePath;
		TextureGenerationData generationData;
	};

	class AssetManager : public Singleton
	{
	public:
		AssetManager();
		~AssetManager();

		static AssetManager& GetInstance();
		bool TexturesInProgress();

		Texture* Load2DTexture(std::string &path, TextureSettings *settings = nullptr);
		Texture* Load2DTextureAsync(std::string &path, TextureSettings *settings = nullptr);

		void Update(int texturesPerFrame);

		inline static Texture* GetWhiteTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetBlackTexture() { return TextureLoader::s_BlackTexture; }
		inline static Texture* GetDefaultNormalTexture() { return TextureLoader::s_DefaultNormal; }
		inline static Texture* GetDefaultAOTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetFullMetallicTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetNoMetallicTexture() { return TextureLoader::s_BlackTexture; }
		inline static Texture* GetFullRoughnessTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetNoRoughnessTexture() { return TextureLoader::s_BlackTexture; }
	private:
		// Used to load resources asynchronously on a threadpool
		void LoaderThread();

		static AssetManager *s_Instance;

		std::vector<std::thread> m_WorkerThreads;
		std::atomic<bool> m_LoadingThreadsActive;
		bool m_AsyncLoadingActive = true;

		std::unordered_map<std::string, Texture*> m_TextureCache;
		ThreadSafeQueue<TextureLoadJob> m_LoadingTexturesQueue;
		ThreadSafeQueue<TextureLoadJob> m_GenerateTexturesQueue;
	};
}
