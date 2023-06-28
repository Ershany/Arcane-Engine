#pragma once
#ifndef LAYER_H
#define LAYER_H

namespace Arcane
{
	class Layer
	{
	public:
		Layer(ARC_DEV_ONLY(const std::string &debugName = "Layer"));
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event &event) {}

		ARC_DEV_ONLY(inline const std::string& GetName() const { return m_DebugName; })
	protected:
		ARC_DEV_ONLY(std::string m_DebugName);
	};
}
#endif
