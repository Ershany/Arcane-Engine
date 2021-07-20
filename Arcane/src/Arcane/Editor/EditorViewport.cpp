#include "arcpch.h"
#include "EditorViewport.h"

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	EditorViewport::EditorViewport()
	{

	}

	void EditorViewport::OnImGuiRender()
	{
		ImGui::Begin("GameWindow");
		{
			// BeginChild allows it to fill all the space of the window
			// Also allows for customization
			ImGui::BeginChild("Game Render");
			ImGui::Image((ImTextureID)0, ImVec2(800.0f, 450.f), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild();
		}
		ImGui::End();
	}
}
