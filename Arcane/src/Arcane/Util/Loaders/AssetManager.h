#pragma once
#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#ifndef THREADSAFEQUEUE_H
#include <Arcane/Core/Threads/ThreadSafeQueue.h>
#endif

#ifndef TEXTURELOADER_H
#include <Arcane/Util/Loaders/TextureLoader.h>
#endif

namespace Arcane
{
	struct TextureSettings;
	class Model;

	struct TextureLoadJob
	{
		std::string texturePath;
		TextureGenerationData generationData;
	};

	struct CubemapLoadJob
	{
		std::string texturePath;
		CubemapGenerationData generationData;
	};

	struct ModelLoadJob
	{
		std::string path;
		Model *model;
	};

	class AssetManager : public Singleton
	{
	public:
		AssetManager();
		~AssetManager();

		static AssetManager& GetInstance();
		inline bool AssetsInFlight() { return m_AssetsInFlight > 0; }

		Model* LoadModel(const std::string &path);
		Model* LoadModelAsync(const std::string &path);

		Texture* Load2DTexture(const std::string &path, TextureSettings *settings = nullptr);
		Texture* Load2DTextureAsync(const std::string &path, TextureSettings *settings = nullptr);

		// TODO: HDR loading
		Cubemap* LoadCubemapTexture(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings = nullptr);
		Cubemap* LoadCubemapTextureAsync(const std::string &right, const std::string &left, const std::string &top, const std::string &bottom, const std::string &back, const std::string &front, CubemapSettings *settings = nullptr);

		void Update(int texturesPerFrame, int cubemapFacesPerFrame, int modelsPerFrame);

		inline static Texture* GetWhiteTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetBlackTexture() { return TextureLoader::s_BlackTexture; }
		inline static Texture* GetDefaultNormalTexture() { return TextureLoader::s_DefaultNormal; }
		inline static Texture* GetDefaultAOTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetFullMetallicTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetNoMetallicTexture() { return TextureLoader::s_BlackTexture; }
		inline static Texture* GetFullRoughnessTexture() { return TextureLoader::s_WhiteTexture; }
		inline static Texture* GetNoRoughnessTexture() { return TextureLoader::s_BlackTexture; }
		inline static Texture* GetDefaultWaterDistortionTexture() { return TextureLoader::s_DefaultWaterDistortion; }
	private:
		// Used to load resources asynchronously on a threadpool
		void LoaderThread();

		std::vector<std::thread> m_WorkerThreads;
		std::atomic<bool> m_LoadingThreadsActive;
		bool m_AsyncLoadingActive = true;

		// Keeps tracks of assets in flight, there can be a gap between the two queues and we need a way to know when all in-flight assets are complete. This is only incremented on asset load (main thread) and decremented on main thread when finishing creating the asset
		int m_AssetsInFlight = 0;

		std::unordered_map<std::string, Texture*> m_TextureCache;
		ThreadSafeQueue<TextureLoadJob> m_LoadingTexturesQueue;
		ThreadSafeQueue<TextureLoadJob> m_GenerateTexturesQueue;

		ThreadSafeQueue<CubemapLoadJob> m_LoadingCubemapQueue;
		ThreadSafeQueue<CubemapLoadJob> m_GenerateCubemapQueue;

		std::unordered_map<std::string, Model*> m_ModelCache;
		ThreadSafeQueue<ModelLoadJob> m_LoadingModelQueue;
		ThreadSafeQueue<ModelLoadJob> m_GenerateModelQueue;
	};
}
#endif
