#pragma once

#include <Arcane/ImGui/ImGuiLayer.h>

namespace Arcane
{
	class OpenGLImGuiLayer : public ImGuiLayer
	{
	public:
		OpenGLImGuiLayer(ARC_DEV_ONLY(const std::string &name));
		virtual ~OpenGLImGuiLayer();

		virtual void Begin() override;
		virtual void End() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		ARC_DEV_ONLY(std::string m_DebugName);
	};
}
