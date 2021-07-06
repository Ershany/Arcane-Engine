#pragma once

#include <Arcane/Core/Assert.h>

#ifdef ARC_PLATFORM_WINDOWS
#else
	#error Arcane only supports Windows
#endif

#define BIT(x) (1u << x)

#define ARC_BIND_EVENT_FN(fn) std::bind(&##fn, this, std::placeholders::_1)
