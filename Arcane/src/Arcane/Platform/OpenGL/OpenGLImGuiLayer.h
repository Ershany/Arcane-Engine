#pragma once

#include <Arcane/ImGui/ImGuiLayer.h>
#include <Arcane/Core/Base.h>

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

#if ARC_DEV_BUILD
		std::string m_DebugName);
#endif
	};
}
