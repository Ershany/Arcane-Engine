#pragma once
#ifndef ASSERT_H
#define ASSERT_H

#ifdef ARC_DEBUG
#define ARC_ASSERT(x, ...) { if(!(x)) { ARC_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#elif defined(ARC_RELEASE)
#define ARC_ASSERT(x, ...) { if(!(x)) { ARC_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); } }
#else
#define ARC_ASSERT(x, ...)
#endif

#endif
