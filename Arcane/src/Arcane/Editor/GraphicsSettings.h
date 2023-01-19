#pragma once

#include <Arcane/Graphics/Renderer/RenderPass/MasterRenderer.h>

namespace Arcane
{
	class GraphicsSettings
	{
	public:
		GraphicsSettings(MasterRenderPass *renderer);

		void OnImGuiRender(bool *pOpen);
	private:
		MasterRenderPass *m_MasterRenderPass;
	};
}
