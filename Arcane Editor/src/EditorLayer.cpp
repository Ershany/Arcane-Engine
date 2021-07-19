#include "EditorLayer.h"

namespace Arcane
{
	EditorLayer::EditorLayer()
	{

	}

	EditorLayer::~EditorLayer()
	{

	}

	void EditorLayer::OnAttach()
	{

	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(float deltaTime)
	{

	}

	void EditorLayer::OnImGuiRender()
	{
		static bool openBool = true;

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGui::Begin("DockSpace Demo", &openBool, window_flags);

		// Setup dock space
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceID = ImGui::GetID("DockingSpace");
		}

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event &event)
	{

	}
}
