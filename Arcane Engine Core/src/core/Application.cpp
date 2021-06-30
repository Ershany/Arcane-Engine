#include "pch.h"
#include "Application.h"

namespace arcane
{
	Application::Application(const ApplicationSpecification &spec)
	{

	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		onInit();

		while (m_Running)
		{


			// Render stuff
			if (!m_Minimized)
			{

			}
		}

		onShutdown();
	}

	void Application::close()
	{
		m_Running = false;
	}

	void Application::onEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->onEvent(event);
			if (event.Handled)
				break;
		}
	}

	void Application::pushLayer(Layer *layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer *overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->onAttach();
	}

	void Application::renderImGui()
	{

	}
}
