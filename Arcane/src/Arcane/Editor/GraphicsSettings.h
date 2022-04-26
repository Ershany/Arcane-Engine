#pragma once

#include <Arcane/Graphics/Renderer/MasterRenderer.h>

namespace Arcane
{
	class GraphicsSettings
	{
	public:
		GraphicsSettings(MasterRenderer *renderer);

		void OnImGuiRender(bool *pOpen);
	private:
		MasterRenderer *m_Renderer;
	};
}
