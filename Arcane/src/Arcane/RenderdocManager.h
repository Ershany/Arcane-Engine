#pragma once

#ifdef USE_RENDERDOC

#define RENDERDOCMANAGER Arcane::RenderdocManager::GetInstance()
#define RENDERDOCAPI RENDERDOCMANAGER.GetRenderdocAPI()

namespace Arcane
{
	class RenderdocManager
	{
	public:
		RenderdocManager(const RenderdocManager&) = delete;
		RenderdocManager& operator=(const RenderdocManager&) = delete;
		RenderdocManager(RenderdocManager&&) = delete;
		RenderdocManager& operator=(RenderdocManager&&) = delete;

		~RenderdocManager();

		static RenderdocManager& GetInstance()
		{
			static RenderdocManager instance;
			return instance;
		}

		RENDERDOC_API_1_5_0* GetRenderdocAPI() { return m_RenderdocAPI; }
		
		void Update();

	private:
		RenderdocManager();

		RENDERDOC_API_1_5_0* m_RenderdocAPI;
	};
}
#endif // USE_RENDERDOC
