#pragma once

#include <Arcane/Core/Assert.h>

#ifdef ARC_PLATFORM_WINDOWS
#else
	#error Arcane only supports Windows
#endif

#define BIT(x) (1u << x)

#define ARC_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype>(args)>(args)...); }

#if defined(ARC_DEBUG) || defined(ARC_RELEASE)
#define ARC_DEV_BUILD
#endif

#ifdef ARC_DEV_BUILD
#define ARC_DEV_ONLY(...) __VA_ARGS__
#else
#define ARC_DEV_ONLY(...)
#endif
