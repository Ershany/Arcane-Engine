#include "arcpch.h"

#include <Arcane/Core/Application.h>

#include <Arcane/vendor/renderdoc-1.x/renderdoc/api/app/renderdoc_app.h>
#include <filesystem>

int main() {

	// Check if there is a post build way of copying the renderdoc.dll to the exe location
	// TODO:Make sure this is under Debug only
	// Setup renderdoc here 
	// TODO: This is to be moved somewhere else
	// PS: Make sure your using the right bit version for your dlls
	// For android replace librenderdoc.so with libVkLayer_GLES_RenderDoc.so
	std::string dir;
	std::filesystem::current_path(dir);

	// renderdoc api
	RENDERDOC_API_1_5_0* m_RenderdocApi = nullptr;
	pRENDERDOC_GetAPI RENDERDOC_GetAPI;
	void* mod = nullptr;

#ifdef _WIN32
	mod = LoadLibrary("renderdoc.dll");
#elif (__Linux__)	
	void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
#endif

	assert(mod);

#ifdef _WIN32
	RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress((HMODULE)mod, "RENDERDOC_GetAPI");
#elif (__linux__)	
	RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
#endif

	int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void**)&m_RenderdocApi);
	assert(ret == 1);

	m_RenderdocApi->SetLogFilePathTemplate((dir + "/logs/").c_str());

	// To start a frame capture, call StartFrameCapture.
	// You can specify NULL, NULL for the device to capture on if you have only one device and
	// either no windows at all or only one window, and it will capture from that device.
	// See the documentation below for a longer explanation
	//if (m_RenderdocApi)
	//	m_RenderdocApi->StartFrameCapture(NULL, NULL);


	// Stop screen capture
	//if (m_RenderdocApi)
	//	m_RenderdocApi->EndFrameCapture(NULL, NULL);


	// Temp: Should be handled by whatever service that wants to run Arcane
	Arcane::ApplicationSpecification specification;
	specification.Name = "Arcane Engine";
	specification.WindowWidth = WINDOW_X_RESOLUTION;
	specification.WindowHeight = WINDOW_Y_RESOLUTION;
	specification.RenderResolutionWidth = specification.WindowWidth * SUPERSAMPLING_FACTOR;
	specification.RenderResolutionHeight = specification.WindowHeight * SUPERSAMPLING_FACTOR;
	specification.VSync = V_SYNC;
	specification.EnableImGui = true;
	new Arcane::Application(specification);

	//Arcane::Application::GetInstance().pushOverlay();
	Arcane::Application::GetInstance().Run();

	return EXIT_SUCCESS;
}
