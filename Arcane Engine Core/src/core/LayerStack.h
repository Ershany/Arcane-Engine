#pragma once

#include <core/Layer.h>

namespace arcane
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack() = default;

		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		void PopLayer(Layer *layer);
		void PopOverlay(Layer *overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}
