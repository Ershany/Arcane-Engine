#pragma once

#include <Arcane/Core/Assert.h>

#ifdef ARC_PLATFORM_WINDOWS
#else
	#error Arcane only supports Windows
#endif

#define BIT(x) (1u << x)

#define ARC_BIND_EVENT_FN(fn) std::bind(&##fn, this, std::placeholders::_1)

#define ARC_RENDERDOC_DEBUG defined(ARC_DEBUG) || defined(ARC_RELEASE)

#if defined(ARC_DEBUG) || defined(ARC_RELEASE)
#define ARC_DEV_BUILD
#endif

#ifdef ARC_DEV_BUILD
#define ARC_DEV_ONLY(...) __VA_ARGS__
#else
#define ARC_DEV_ONLY(...)
#endif


typedef uint8_t u8;

