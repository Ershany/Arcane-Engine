#pragma once

#include <core/LayerStack.h>
#include <utils/Singleton.h>

namespace arcane
{
	class Layer;

	struct ApplicationSpecification
	{
		std::string Name = "Arcane";
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
		LayerStack m_LayerStack;

		bool m_Running = true;
		bool m_Minimized = false;

		static Application *s_Instance;
	};

	// Implemented by the client
	Application* CreateApplication(int argc, char** argv);
}
