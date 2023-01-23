#include <Arcane.h>
#include <Arcane/ArcaneEntryPoint.h>

#include <EditorLayer.h>

class ArcaneEditorApplication : public Arcane::Application
{
public:
	ArcaneEditorApplication(const Arcane::ApplicationSpecification &specification)
		: Application(specification)
	{

	}

	~ArcaneEditorApplication()
	{

	}

	virtual void OnInit() override
	{
		PushLayer(new Arcane::EditorLayer());
	}
};

Arcane::Application* Arcane::CreateApplication(int argc, char **argv)
{
	// Attach RenderDoc for debug builds
#if 0
#if ARC_DEBUG
	RENDERDOC_API_1_5_0* m_RenderdocApi = nullptr;

	if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_5_0, (void **)&m_RenderdocApi);
		assert(ret == 1);
	}

	if (m_RenderdocApi)
	{
		ARC_LOG_INFO("RenderDoc successfully loaded!");
	}
	/*
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
	*/

	// To start a frame capture, call StartFrameCapture.
	// You can specify NULL, NULL for the device to capture on if you have only one device and
	// either no windows at all or only one window, and it will capture from that device.
	// See the documentation below for a longer explanation
	//if (m_RenderdocApi)
	//	m_RenderdocApi->StartFrameCapture(NULL, NULL);


	// Stop screen capture
	//if (m_RenderdocApi)
	//	m_RenderdocApi->EndFrameCapture(NULL, NULL);
#endif
#endif

	Arcane::ApplicationSpecification specification;
	specification.Name = "Arcane Editor";
	specification.WindowWidth = 2560;
	specification.WindowHeight = 1440;
	specification.RenderResolutionWidth = 1920 * SUPERSAMPLING_FACTOR;
	specification.RenderResolutionHeight = 1080 * SUPERSAMPLING_FACTOR;
	specification.VSync = true;
	specification.EnableImGui = true;

	// Create the app and set the renderer to not present the final buffer to the window's swapchain since the editor is going to be in charge of displaying the viewport
	ArcaneEditorApplication *app = new ArcaneEditorApplication(specification);
	app->GetRenderer()->SetRenderToSwapchain(false);
	return app;
}
