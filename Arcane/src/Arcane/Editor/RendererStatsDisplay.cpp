#include "arcpch.h"
#include "RendererStatsDisplay.h"

#include <Arcane/Vendor/Imgui/imgui.h>
#include <Arcane/Graphics/Renderer/Renderer.h>

namespace Arcane
{
	RendererStatsDisplay::RendererStatsDisplay()
	{

	}

	void RendererStatsDisplay::OnImGuiRender()
	{
		const RendererData &rendererStats = Arcane::Renderer::GetRendererData();

		ImGui::Begin("Renderer Stats");
		{
			ImGui::Text("Total Draw Call Count: %u", rendererStats.DrawCallCount);
			ImGui::Text("Mesh Draw Call Count: %u", rendererStats.MeshesDrawnCount);
			ImGui::Text("Quads Draw Call Count: %u", rendererStats.QuadsDrawnCount);
		}
		ImGui::End();
	}
}
