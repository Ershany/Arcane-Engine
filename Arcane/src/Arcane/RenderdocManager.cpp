
#if ARC_RENDERDOC_DEBUG

#include "arcpch.h"
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
		void* mod = nullptr;

	#ifdef _WIN32
		const char* rendDLL = "renderdoc.dll";
		mod = LoadLibraryA(rendDLL);
	#elif (__Linux__)	
		void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
	#endif

		ARC_ASSERT(mod);

	#ifdef _WIN32
		RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress((HMODULE)mod, "RENDERDOC_GetAPI");
	#elif (__linux__)	
		RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
	#endif

		ARC_ASSERT(RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void**)&RENDERDOCAPI) == 1);

		// Make sure the renderdoc log file is there, create it otherwise
		std::string logDir = dir.string() + "/logs/";
		if (!std::filesystem::exists(logDir))
		{
			std::filesystem::create_directory(logDir);
		}

		RENDERDOCAPI->SetLogFilePathTemplate(logDir.c_str());
	}

	RenderdocManager::~RenderdocManager()
	{
		m_RenderdocAPI->Shutdown();
	}
}
#endif // ARC_RENDERCOC_DEBUG
