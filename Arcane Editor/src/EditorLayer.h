#pragma once

#include <Arcane.h>

namespace Arcane
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event &event) override;
	};
}
