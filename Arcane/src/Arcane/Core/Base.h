#pragma once
#ifndef BASE_H
#define BASE_H

#ifdef ARC_PLATFORM_WINDOWS
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WGL
#else
	#error Arcane only supports Windows
#endif

#define BIT(x) (1u << x)

#define ARC_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype>(args)>(args)...); }

#if defined(ARC_DEBUG) || defined(ARC_RELEASE)
	#define ARC_DEV_BUILD
#endif

#if defined(USE_RENDERDOC) && defined(ARC_DEV_BUILD)
	#define ARC_PUSH_RENDER_TAG(name) glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name);
	#define ARC_POP_RENDER_TAG() glPopDebugGroup();
#else
	#define ARC_PUSH_RENDER_TAG(name)
	#define ARC_POP_RENDER_TAG()
#endif

#ifdef ARC_DEV_BUILD
	#define ARC_GPU_TIMER_BEGIN(timer) GPUTimerManager::BeginQuery(timer)
	#define ARC_GPU_TIMER_END(timer) GPUTimerManager::EndQuery(timer)
#else
	#define ARC_GPU_TIMER_BEGIN(timer) (void)0
	#define ARC_GPU_TIMER_END(timer) (void)0
#endif

#ifdef ARC_DEV_BUILD
	#define ARC_DEV_ONLY(...) __VA_ARGS__
#else
	#define ARC_DEV_ONLY(...)
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#endif
