#pragma once

// Debug Options
#define DEBUG_ENABLED 1


// Window Settings
#define WINDOW_X_RESOLUTION 1366
#define WINDOW_Y_RESOLUTION 768
#define V_SYNC 1
#define FULLSCREEN_MODE 0 // If set, window resolution is maximized to your screen resolution

// Render Settings
#define FORWARD_RENDER 0

// AA Settings
#define MSAA_SAMPLE_AMOUNT 4 // Only used in forward rendering
#define SUPERSAMPLING_FACTOR 1 // 1 means window resolution will be the render resolution

// Texture Filtering Settings
#define ANISOTROPIC_FILTERING_LEVEL 8.0f

// IBL Settings
#define LIGHT_PROBE_RESOLUTION 32
#define REFLECTION_PROBE_MIP_COUNT 5
#define REFLECTION_PROBE_RESOLUTION 128
#define IBL_CAPTURE_RESOLUTION 256 // Should always be greater than the light and reflection probe resolution
#define BRDF_LUT_RESOLUTION 512

// Frustum Options
#define NEAR_PLANE 0.1f
#define FAR_PLANE 2000.0f

// Shadowmap Options
#define SHADOWMAP_RESOLUTION_X 2048
#define SHADOWMAP_RESOLUTION_Y 2048
#define SHADOWMAP_NEAR_PLANE 1.0f
#define SHADOWMAP_FAR_PLANE 400.0f

// SSAO Options
#define SSAO_KERNEL_SIZE 32 // Maximum amount is restricted by the shader. Only supports a maximum of 64

// Parallax Options
#define PARALLAX_MIN_STEPS 1
#define PARALLAX_MAX_STEPS 20
