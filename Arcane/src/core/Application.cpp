#include "arcpch.h"
#include "Application.h"

#include <Defs.h>
#include <graphics/Window.h>
#include <graphics/renderer/MasterRenderer.h>
#include <scene/Scene3D.h>
#include <utils/loaders/TextureLoader.h>
#include <utils/Time.h>
#include <ui/DebugPane.h>
#include <ui/RuntimePane.h>
#include <ui/WaterPane.h>

namespace Arcane
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification &spec) : m_Specification(spec)
	{
		s_Instance = this;

		// Prepare the engine
		ARC_LOG_INFO("Initializing Arcane Engine...");
		m_Window = new Window(spec);
		m_Window->Init();
		Arcane::TextureLoader::InitializeDefaultTextures();
		m_Scene3D = new Scene3D(m_Window);
		m_Renderer = new MasterRenderer(m_Scene3D);
		m_Manager = new InputManager();

		// Initialize the renderer
		m_Renderer->Init();
	}

	Application::~Application()
	{
		for (Layer *layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		// Todo: insert render queue flush here
		// And render shutdown

		delete m_Window;
		delete m_Scene3D;
		delete m_Renderer;
		delete m_Manager;
	}

	void Application::Run()
	{
		OnInit();

		// Temp ImGui Windows
		Arcane::RuntimePane runtimePane(glm::vec2(270.0f, 175.0f));
		Arcane::DebugPane debugPane(glm::vec2(270.0f, 400.0f));
		Arcane::WaterPane waterPane(glm::vec2(270.0f, 400.0f));

		uint64_t frameCounter = 0;
		Time deltaTime;
		while (m_Running && !m_Window->Closed())
		{
			deltaTime.Update();

			m_Window->Update();

			// Render stuff
			if (!m_Minimized)
			{
				// Wireframe stuff
				#ifdef ARC_DEV_BUILD
					if (debugPane.GetWireframeMode())
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					else
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				#endif // ARC_DEV_BUILD

				m_Window->Bind();
				m_Window->Clear();
				ImGui_ImplGlfwGL3_NewFrame();

				//for (Layer *layer : m_LayerStack)
					//layer->onUpdate()
				m_Scene3D->OnUpdate((float)deltaTime.GetDeltaTime());
				m_Renderer->Render();

				// Display panes
				if (!Arcane::Window::GetHideUI())
				{
					Arcane::Window::Bind();
					runtimePane.Render();
					debugPane.Render();
					waterPane.Render();
				}
				ImGui::Render();
				ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

				++frameCounter;
			}
		}

		OnShutdown();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event &event)
	{
		EventDispatcher dispatcher(event);
		
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(event);
			if (event.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer *layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer *overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::RenderImGui()
	{

	}

	const char* Application::GetConfigName()
	{
#if defined(ARC_DEBUG)
		return "Debug";
#elif defined(ARC_RELEASE)
		return "Release";
#elif defined(ARC_FINAL)
		return "Final";
#else
		#error Undefined Config
#endif
	}

	const char* Application::GetPlatformName()
	{
#if defined(ARC_PLATFORM_WINDOWS)
		return "Windows";
#else
		#error Undefined Platform
#endif
	}
}
