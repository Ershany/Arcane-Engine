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
	specification.WindowWidth = 1920;
	specification.WindowHeight = 1080;
	specification.VSync = true;
	specification.EnableImGui = true;
	return new ArcaneEditorApplication(specification);
}
