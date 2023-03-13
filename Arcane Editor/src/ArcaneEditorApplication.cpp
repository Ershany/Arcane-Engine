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
	app->GetMasterRenderPass()->SetRenderToSwapchain(false);
	return app;
}
