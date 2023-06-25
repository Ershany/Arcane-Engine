#include "arcpch.h"
#include "RendererStatsDisplay.h"

#include <Arcane/Vendor/Imgui/imgui.h>
#include <Arcane/Graphics/Renderer/Renderer.h>

#ifdef ARC_DEV_BUILD
#include <Arcane/Platform/OpenGL/GPUTimerManager.h>
#endif

namespace Arcane
{
	RendererStatsDisplay::RendererStatsDisplay()
	{

	}

	void RendererStatsDisplay::OnImGuiRender()
	{
		const RendererData &rendererStats = Renderer::GetRendererData();

		ImGui::Begin("Renderer Stats");
		{
			ImGui::Text("Total Draw Call Count: %u", rendererStats.DrawCallCount);
			ImGui::Text("Mesh Draw Call Count: %u", rendererStats.MeshesDrawnCount);
			ImGui::Text("Quads Draw Call Count: %u", rendererStats.QuadsDrawnCount);
			ImGui::Separator();
#ifdef ARC_DEV_BUILD
			float frametime = 1000.0f / ImGui::GetIO().Framerate;
			ImGui::Text("Frametime: %.3f ms (FPS %.1f)", frametime, ImGui::GetIO().Framerate);
			GPUTimerManager::BuildImguiTimerUI();
#endif
		}
		ImGui::End();
	}
}
