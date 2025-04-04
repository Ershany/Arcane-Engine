#include "arcpch.h"
#include "Application.h"

#include <Arcane/Graphics/Window.h>
#include <Arcane/Graphics/Renderer/Renderer.h>
#include <Arcane/Graphics/Renderer/RenderPass/MasterRenderPass.h>
#include <Arcane/Scene/Scene.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Util/Loaders/ShaderLoader.h>
#include <Arcane/Util/Loaders/TextureLoader.h>
#include <Arcane/Util/Time.h>
#include <Arcane/Core/Layer.h>
#include <Arcane/ImGui/ImGuiLayer.h>
#include <Arcane/RenderdocManager.h>
#include <Arcane/Input/InputManager.h>
#include <Arcane/Graphics/Renderer/DebugDraw3D.h>

#include "glfw/glfw3native.h"

#ifdef ARC_DEV_BUILD
#include <Arcane/Platform/OpenGL/GPUTimerManager.h>
#endif


extern bool g_ApplicationRunning;
namespace Arcane
{
#define BIND_EVENT_FN(fn) std::bind(&Application::##fn, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification &specification) : m_Specification(specification), m_Wireframe(false)
	{
		s_Instance = this;

		// Prepare the engine
		ARC_LOG_INFO("Initializing Arcane Engine...");
		m_Window = new Window(this, specification);
		m_Window->Init();
		m_AssetManager = &Arcane::AssetManager::GetInstance(); // Need to initialize the asset manager early so we can load resources and have our worker threads instantiated
		Arcane::ShaderLoader::SetShaderFilepath("../Arcane/src/Arcane/shaders/");
		Renderer::Init(); // Must be loaded before textures get created since they query for the max anistropy from the renderer
		Arcane::TextureLoader::InitializeDefaultTextures();
		m_ActiveScene = new Scene(m_Window);
		m_MasterRenderPass = new MasterRenderPass(m_ActiveScene);
		m_InputManager = &InputManager::GetInstance();
	}

	Application::~Application()
	{
		for (Layer *layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

#ifdef ARC_DEV_BUILD
		GPUTimerManager::Shutdown();
#endif

		Renderer::Shutdown();

		delete m_Window;
		delete m_ActiveScene;
		delete m_MasterRenderPass;
	}

	void Application::InternalInit()
	{
		// This will call OnAttach for any layers in the layer stack. This is where the editor layer can load up assets before runtime
		OnInit();

		// Make sure all assets load before booting for first time
		while (Arcane::AssetManager::GetInstance().AssetsInFlight())
		{
			m_AssetManager->Update(100000, 100000, 100000);
		}

		m_ActiveScene->Init();

		// Initialize the master render pass
		m_MasterRenderPass->Init();

#ifdef ARC_DEV_BUILD
		GPUTimerManager::Startup();
#endif

		if (m_Specification.EnableImGui)
		{
			m_ImGuiLayer = ImGuiLayer::Create(ARC_DEV_ONLY("Engine ImGui Layer"));
			PushOverlay(m_ImGuiLayer);
		}
	}

	void Application::Run()
	{
		InternalInit();

		uint64_t frameCounter = 0;
		Time deltaTime;
		while (m_Running && !m_Window->Closed())
		{
			deltaTime.Update();
			m_InputManager->Update();
			m_Window->Update();

#if USE_RENDERDOC
			RENDERDOCMANAGER.Update();
#endif
			// Render stuff
			if (!m_Minimized)
			{
				// Wireframe stuff
				if (m_Wireframe)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				m_Window->Bind();
				m_Window->ClearAll();

				m_AssetManager->Update(TEXTURE_LOADS_PER_FRAME, CUBEMAP_FACES_PER_FRAME, MODELS_PER_FRAME);
				m_ActiveScene->OnUpdate((float)deltaTime.GetDeltaTime());

				for (Layer *layer : m_LayerStack)
					layer->OnUpdate((float)deltaTime.GetDeltaTime());

				// Render the frame
				Renderer::BeginFrame();
				{
					m_MasterRenderPass->Render();
				}
				Renderer::EndFrame();

				// Render ImGui Layers On-top
				if (m_Specification.EnableImGui)
					RenderImGui();

#ifdef ARC_DEV_BUILD
				GPUTimerManager::EndOfFrameUpdate();
#endif

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
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		
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
		m_ImGuiLayer->Begin();

		for (Layer *layer : m_LayerStack)
			layer->OnImGuiRender();

		m_ImGuiLayer->End();
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

	bool Application::OnWindowClose(WindowCloseEvent &event)
	{
		ARC_LOG_INFO("Shutting down engine..");
		m_Running = false;
		g_ApplicationRunning = false;

		return true;
	}
}
