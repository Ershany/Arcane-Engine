#pragma once

namespace arcane
{
	class Layer
	{
	public:
		Layer(const std::string &debugName = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(float deltaTime) {}
		virtual void onImGuiRender() {}
		//virtual void onEvent(Event &event) {}

		inline const std::string& getName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
