#include "EditorLayer.h"

#include <Arcane/Core/Application.h>
#include <Arcane/Graphics/Renderer/Renderpass/EditorPass.h>
#include <Arcane/Vendor/Imgui/imgui.h>
#include <Testbed.h>

extern bool g_ApplicationRunning;
namespace Arcane
{
	EditorLayer::EditorLayer(const std::string &debugName /*= "Layer"*/) : Layer(debugName), m_EditorScene(Arcane::Application::GetInstance().GetScene()), m_EditorViewport(), m_ConsolePanel(), m_GraphicsSettings(Arcane::Application::GetInstance().GetMasterRenderPass()),
		m_InspectorPanel(), m_ScenePanel(m_EditorScene, &m_InspectorPanel), m_ShowGraphicsSettings(false)
	{}

	EditorLayer::~EditorLayer()
	{

	}

	void EditorLayer::OnAttach()
	{
		Testbed::LoadTestbedGraphics();
		//Testbed::LoadTestbedAnimation();
		//Testbed::LoadTestbedGraphics2D();

#ifdef OLD_LOADING_METHOD
		//Model *simpsonsBuilding = new Arcane::Model("res/3D_Models/Simpsons/MoesTavern.obj");
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(20.0f, 15.0f, 30.0f), glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f), simpsonsBuilding, nullptr, false, false));

		//Model *sponza = new Arcane::Model("res/3D_Models/Sponza/sponza.obj");
		//m_RenderableModels.push_back(new RenderableModel(glm::vec3(67.0f, 110.0f, 133.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(180.0f), sponza, nullptr, true, false));
#endif
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		Arcane::Application::GetInstance().GetMasterRenderPass()->GetEditorPass()->SetFocusedEntity(m_InspectorPanel.GetFocusedEntity());
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();


		static bool p_open = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("New Scene", "Ctrl+N", false, false);
				ImGui::MenuItem("Open Scene...", "Ctrl+O", false, false);
				ImGui::Separator();
				ImGui::MenuItem("Save", "Ctrl+S", false, false);
				ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, false);
				ImGui::Separator();
				if (ImGui::MenuItem("Exit"))
				{
					Application::GetInstance().Close();
					g_ApplicationRunning = false;
				}
				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::MenuItem("Undo", NULL, false, false);
				ImGui::MenuItem("Redo", NULL, false, false);
				ImGui::Separator();
				ImGui::MenuItem("Cut", NULL, false, false);
				ImGui::MenuItem("Copy", NULL, false, false);
				ImGui::MenuItem("Paste", NULL, false, false);
				ImGui::Separator();
				ImGui::MenuItem("Play", NULL, false, false);
				ImGui::MenuItem("Pause", NULL, false, false);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::MenuItem("Asset Manager", NULL, false, false);
				ImGui::Separator();
				ImGui::MenuItem("Graphics Settings", NULL, &m_ShowGraphicsSettings);
				ImGui::MenuItem("Physics Settings", NULL, false, false);
				ImGui::MenuItem("Project Settings", NULL, false, false);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("About Arcane", NULL, false, false);
				ImGui::Separator();
				ImGui::MenuItem("Arcane Documentation", NULL, false, false);
				ImGui::Separator();
				ImGui::MenuItem("Report a Bug", NULL, false, false);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_EditorViewport.OnImGuiRender();
		m_ConsolePanel.OnImGuiRender();
		m_ScenePanel.OnImGuiRender();
		m_InspectorPanel.OnImGuiRender();
		m_RendererStatsDisplay.OnImGuiRender();
		if (m_ShowGraphicsSettings) m_GraphicsSettings.OnImGuiRender(&m_ShowGraphicsSettings);

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event &event)
	{

	}

	void EditorLayer::NewScene()
	{

	}

	void EditorLayer::OpenScene(const std::string& filepath)
	{
		// TODO: Open scene and set the current/editor scene to point to it
	}
}
