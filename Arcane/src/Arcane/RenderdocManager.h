#if ARC_RENDERDOC_DEBUG
#pragma once


#include <Arcane/Core/Base.h>

#define RENDERDOCAPI Arcane::RenderdocManager::GetRenderdocAPI();
#define RENDERDOCMANAGER Arcane::RenderdocManager::GetInstance();

namespace Arcane
{
	class RenderdocManager
	{
	public:
		RenderdocManager() = delete;
		RenderdocManager(const RenderdocManager&) = delete;
		operator=(const RenderdocManager&) = delete;
		RenderdocManager(RenderdocManager&&) = delete;
		operator=(RenderdocManager&&) = delete;

		~RenderdocManager();

		static RenderdocManager& GetInstance()
		{
			static RenderdocManager s_RenderdocManager;
			return s_RenderdocManager;
		}

		static RENDERDOC_API_1_5_0* GetRenderdocAPI() { return m_RenderdocAPI; }

	private:
		RenderdocManager();

		RENDERDOC_API_1_5_0* m_RenderdocAPI;
	};
}
#endif
