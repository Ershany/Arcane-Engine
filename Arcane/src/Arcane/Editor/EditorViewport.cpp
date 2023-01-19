#include "arcpch.h"
#include "EditorViewport.h"

#include <Arcane/Core/Application.h>
#include <Arcane/Graphics/Renderer/RenderPass/MasterRenderer.h>
#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	EditorViewport::EditorViewport()
	{

	}

	void EditorViewport::OnImGuiRender()
	{
		static ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
		ImGui::SetNextWindowClass(&window_class);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Game Window");
		{
			ImVec2 imguiWindowSize = ImGui::GetWindowSize();

			// BeginChild allows it to fill all the space of the window
			// Also allows for customization
			ImGui::BeginChild("Game Render");
			ImGui::Image((ImTextureID)Application::GetInstance().GetMasterRenderPass()->GetFinalOutputTexture()->GetTextureId(), imguiWindowSize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
}
