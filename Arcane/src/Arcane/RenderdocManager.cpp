#include "arcpch.h"

#ifdef USE_RENDERDOC

#include <Arcane/RenderdocManager.h>
#include <Arcane/Input/InputManager.h>

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

	void RenderdocManager::Update()
	{
		/**
		 * You can specify NULL, NULL for the device to capture on if you have only one device and
		 * either no windows at all or only one window, and it will capture from that device.
		 * See the documentation below for a longer explanation
		 * NOTE: MIGHT NOT BE A BAD IDEA TO ADD A UI INDICATION THAT WE ARE CAPTURING
		 */
		if (InputManager::IsKeyPressedDown(GLFW_KEY_F9) && !RENDERDOCAPI->IsFrameCapturing())
		{
			ARC_LOG_INFO("Started renderdoc frame capture");
			RENDERDOCAPI->StartFrameCapture(NULL, NULL);
			ARC_ASSERT(RENDERDOCAPI->IsFrameCapturing(), "Trying a renderdoc capture while capturing");
		}

		if (InputManager::IsKeyPressedDown(GLFW_KEY_F10) && RENDERDOCAPI->IsFrameCapturing())
		{
			ARC_LOG_INFO("Ended renderdoc frame capture");
			RENDERDOCAPI->EndFrameCapture(NULL, NULL);
			ARC_ASSERT(!RENDERDOCAPI->IsFrameCapturing(), "Didn't stop capturing after ending capture");
		}

		if (InputManager::IsKeyPressedDown(GLFW_KEY_F11) && !RENDERDOCAPI->IsFrameCapturing())
		{
			ARC_LOG_INFO("Getting a 1 frame renderdoc capture");
			RENDERDOCAPI->TriggerCapture();
		}
	}
}
#endif // USE_RENDERDOC
