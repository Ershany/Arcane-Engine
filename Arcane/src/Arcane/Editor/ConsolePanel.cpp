#include "arcpch.h"
#include "ConsolePanel.h"

#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	ConsolePanel::ConsolePanel()
	{

	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::Begin("Console Log");
		{
			//RenderMenu
			//ImGui::Separator();
			//RenderConsole();
		}
		ImGui::End();
	}

	void ConsolePanel::RenderMenu()
	{

	}

	void ConsolePanel::RenderConsole()
	{
		ImGui::BeginChild("Log Messages");
		{

		}
		ImGui::EndChild();
	}
}
