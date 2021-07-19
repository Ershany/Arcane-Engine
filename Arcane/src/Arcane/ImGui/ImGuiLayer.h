#pragma once

#include <Arcane/Core/Layer.h>

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
