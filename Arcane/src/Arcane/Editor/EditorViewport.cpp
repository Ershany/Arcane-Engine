#include "arcpch.h"
#include "EditorViewport.h"

#include <Arcane/Core/Application.h>
#include <Arcane/Graphics/Renderer/RenderPass/MasterRenderPass.h>
#include <Arcane/Util/Loaders/AssetManager.h>
#include <Arcane/Vendor/Imgui/imgui.h>

namespace Arcane
{
	EditorViewport::EditorViewport() : m_PlaySelected(false)
	{
		AssetManager &assetManager = AssetManager::GetInstance();
		m_PlayTexture = assetManager.Load2DTexture("res/editor/play.png");
		m_StopTexture = assetManager.Load2DTexture("res/editor/stop.png");
		m_PauseTexture = assetManager.Load2DTexture("res/editor/pause.png");
	}

	void EditorViewport::OnImGuiRender()
	{
		static ImGuiWindowClass playStopWindow;
		playStopWindow.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
		ImGui::SetNextWindowClass(&playStopWindow);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Start & Stop");
		{
			Texture *leftImage = m_PlayTexture;
			if (m_PlaySelected)
				leftImage = m_StopTexture;

			float totalWidth = ImGui::GetContentRegionAvailWidth();
			float arbitraryAdjustment = 31.0f;
			ImGui::SetCursorPosX((totalWidth / 2.0f) - arbitraryAdjustment);
			if (ImGui::ImageButton((ImTextureID)leftImage->GetTextureId(), ImVec2(25.0f, 25.0f), ImVec2(0, 0), ImVec2(1, 1), 1))
			{
				if (m_PlaySelected)
				{
					// Start Sim
				}
				else
				{
					// Stop Sim
				}

				m_PlaySelected = !m_PlaySelected;
			}
			ImGui::SameLine();

			if (m_PlaySelected)
			{
				if (ImGui::ImageButton((ImTextureID)m_PauseTexture->GetTextureId(), ImVec2(25.0f, 25.0f), ImVec2(0, 0), ImVec2(1, 1), 1))
				{
					// Pause Sim
				}
			}
			else
			{
				ImGui::Image((ImTextureID)m_PauseTexture->GetTextureId(), ImVec2(25.0f, 25.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0.1922f, 0.3255f, 0.4823f, 1));
			}

		}
		ImGui::End();
		ImGui::PopStyleVar();

		static ImGuiWindowClass viewportWindow;
		viewportWindow.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
		ImGui::SetNextWindowClass(&viewportWindow);
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
