#pragma once
#ifndef GRAPHICSSETTINGS_H
#define GRAPHICSSETTINGS_H

namespace Arcane
{
	class MasterRenderPass;

	class GraphicsSettings
	{
	public:
		GraphicsSettings(MasterRenderPass *renderer);

		void OnImGuiRender(bool *pOpen);
	private:
		MasterRenderPass *m_MasterRenderPass;
	};
}
#endif
