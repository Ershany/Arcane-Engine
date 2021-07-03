#pragma once

#include <core/LayerStack.h>
#include <utils/Singleton.h>

namespace Arcane
{
	class Window;
	class Layer;
	class Scene3D;
	class MasterRenderer;
	class InputManager;

	struct ApplicationSpecification
	{
		std::string Name = "Arcane";
		uint32_t WindowWidth = 1920, WindowHeight = 1080;
		uint32_t RenderResolutionWidth = WindowWidth, RenderResolutionHeight = WindowHeight;
		bool VSync = true;
		bool EnableImGui = true;
	};

	class Application : public Singleton
	{
	public:
		Application(const ApplicationSpecification &spec);
		virtual ~Application();

		static inline Application& GetInstance() { return *s_Instance; }

		void run();
		void close();

		virtual void onInit() {}
		virtual void onShutdown() {}
		virtual void onUpdate(float deltaTime) {}

		virtual void onEvent(Event &event);

		void pushLayer(Layer *layer);
		void pushOverlay(Layer *overlay);
		void renderImGui();

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
	private:
		ApplicationSpecification m_Specification;

		Window *m_Window;
		Scene3D *m_Scene3D;
		MasterRenderer *m_Renderer;
		InputManager *m_Manager;
		LayerStack m_LayerStack;

		bool m_Running = true;
		bool m_Minimized = false;

		static Application *s_Instance;
	};

	// Implemented by the client
	Application* CreateApplication(int argc, char** argv);
}
