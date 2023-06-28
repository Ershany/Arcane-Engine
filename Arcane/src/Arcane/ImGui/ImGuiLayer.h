#pragma once
#ifndef IMGUILAYER_H
#define IMGUILAYER_H

#ifndef LAYER_H
#include <Arcane/Core/Layer.h>
#endif

namespace Arcane
{
	class ImGuiLayer : public Layer
	{
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;

		static ImGuiLayer* Create(ARC_DEV_ONLY(const std::string &name));
	};
}
#endif
