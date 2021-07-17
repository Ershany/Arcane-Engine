#include "arcpch.h"
#include "Layer.h"

namespace Arcane
{
	Layer::Layer(ARC_DEV_ONLY(const std::string &debugName)) ARC_DEV_ONLY(: m_DebugName(debugName))
	{
	}

	Layer::~Layer()
	{
	}
}
