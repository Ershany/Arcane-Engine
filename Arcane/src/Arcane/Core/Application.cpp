#include "arcpch.h"
#include "Application.h"

#include <Arcane/Defs.h>
#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Renderer/MasterRenderer.h>
#include <Arcane/Scene/Scene3D.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Util/Loaders/TextureLoader.h>
#include <Arcane/Util/Time.h>
#include <Arcane/Core/Layer.h>

namespace Arcane
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification &specification) : m_Specification(specification), m_RuntimePane(glm::vec2(270.0f, 175.0f)), m_DebugPane(glm::vec2(270.0f, 400.0f)), m_WaterPane(glm::vec2(270.0f, 400.0f))
	{
		s_Instance = this;

		// Prepare the engine
		ARC_LOG_INFO("Initializing Arcane Engine...");
		m_Window = new Window(specification);
		m_Window->Init();
		Arcane::ShaderLoader::SetShaderFilepath("../Arcane/src/Arcane/shaders/");
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
					if (m_DebugPane.GetWireframeMode())
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					else
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				#endif // ARC_DEV_BUILD

				m_Window->Bind();
				m_Window->Clear();

				m_Scene3D->OnUpdate((float)deltaTime.GetDeltaTime());
				for (Layer *layer : m_LayerStack)
					layer->OnUpdate((float)deltaTime.GetDeltaTime());

				m_Renderer->Render();
				RenderImGui();

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
		ImGui_ImplGlfwGL3_NewFrame();
		if (!Arcane::Window::GetHideUI())
		{
			Arcane::Window::Bind();
			m_RuntimePane.Render();
			m_DebugPane.Render();
			m_WaterPane.Render();
		}
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		for (Layer *layer : m_LayerStack)
			layer->OnImGuiRender();
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
