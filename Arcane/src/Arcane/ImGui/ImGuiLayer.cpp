#include "arcpch.h"
#include "ImGuiLayer.h"

#include <Arcane/Platform/OpenGL/OpenGLImGuiLayer.h>

namespace Arcane
{
	ImGuiLayer* ImGuiLayer::Create(ARC_DEV_ONLY(const std::string &name))
	{
		// Todo: Query which renderapi is being used and create the proper layer, assert if no API is selected
		return new OpenGLImGuiLayer(ARC_DEV_ONLY(name));
	}
}
