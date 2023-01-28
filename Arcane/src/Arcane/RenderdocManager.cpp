#include "arcpch.h"

#ifdef ARC_RENDERDOC_DEBUG

#include <Arcane/RenderdocManager.h>

namespace Arcane
{
	RenderdocManager::RenderdocManager()
	{
		/**
		 * RENDERDOC DLL MUST exist in EXE LOCATION
		 * Check if there is a post build way of copying the renderdoc.dll to the exe location
		 * For android replace librenderdoc.so with libVkLayer_GLES_RenderDoc.so
		 */
		std::filesystem::path dir = std::filesystem::current_path();
		pRENDERDOC_GetAPI RENDERDOC_GetAPI;
		void* mod;

	#ifdef ARC_PLATFORM_WINDOWS
		const char* rendDLL = "renderdoc.dll";
		mod = LoadLibraryA(rendDLL);
	#elif (__Linux__)	
		void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
	#endif

		ARC_ASSERT(mod, "Couldn't load renderdoc dll. Make sure that you have the renderdoc dll in your exe and/or running the game in windows");

	#ifdef _WIN32
		RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress((HMODULE)mod, "RENDERDOC_GetAPI");
	#elif (__linux__)	
		RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
	#endif

		ARC_ASSERT(RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void**)&m_RenderdocAPI) == 1, "Either the renderdoc dll failed to load or something really bad happened here");

		// Make sure the renderdoc log file is there, create it otherwise
		std::string logDir = dir.string() + "/logs/";
		if (!std::filesystem::exists(logDir))
		{
			std::filesystem::create_directory(logDir);
		}

		m_RenderdocAPI->SetLogFilePathTemplate(logDir.c_str());
	}

	RenderdocManager::~RenderdocManager()
	{
		m_RenderdocAPI->Shutdown();
	}
}
#endif // ARC_RENDERCOC_DEBUG
