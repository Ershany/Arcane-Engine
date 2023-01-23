#pragma once

#ifdef ARC_PLATFORM_WINDOWS

#include <arcpch.h>
#include <Arcane/Core/Application.h>

extern Arcane::Application* Arcane::CreateApplication(int argc, char **argv);
bool g_ApplicationRunning = true;

int main(int argc, char **argv)
{

	// Load renderdoc api
// NOTE: RENDERDOC DLL MUST BE COPIED TO EXE LOCATION

// Check if there is a post build way of copying the renderdoc.dll to the exe location
// For android replace librenderdoc.so with libVkLayer_GLES_RenderDoc.so
	std::filesystem::path dir = std::filesystem::current_path();
	pRENDERDOC_GetAPI RENDERDOC_GetAPI;
	void* mod = nullptr;

#ifdef _WIN32
	const char* rendDLL = "renderdoc.dll";
	mod = LoadLibraryA(rendDLL);
#elif (__Linux__)	
	void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
#endif

	assert(mod);

#ifdef _WIN32
	RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress((HMODULE)mod, "RENDERDOC_GetAPI");
#elif (__linux__)	
	RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
#endif

	assert(RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_4_0, (void**)&Arcane::Application::s_RenderdocApi) == 1);

	//auto device = glfwGetWin32Window(m_Window->GetNativeWindow());
	//m_RenderdocApi->SetActiveWindow(device, m_Window->GetNativeWindow());

	std::string logDir = dir.string() + "/logs/";
	if (!std::filesystem::exists(logDir))
	{
		std::filesystem::create_directory(logDir);
	}

	Arcane::Application::s_RenderdocApi->SetLogFilePathTemplate(logDir.c_str());

	while (g_ApplicationRunning)
	{
		Arcane::Application *app = Arcane::CreateApplication(argc, argv);
		ARC_ASSERT(app, "Application is null!");
		app->Run();
		delete app;
	}
}

#endif
