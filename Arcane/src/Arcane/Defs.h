#pragma once

// Debug Options
#define USE_RENDERDOC 0
#define USE_OPENGL_DEBUG 1


// Window Settings
#define WINDOW_X_RESOLUTION 2560
#define WINDOW_Y_RESOLUTION 1440
#define V_SYNC 0
#define FULLSCREEN_MODE 0 // If set, window resolution is maximized to your screen resolution

// Render Settings
#define FORWARD_RENDER 0

// Streaming Settings
#define TEXTURE_LOADS_PER_FRAME 2
#define CUBEMAP_FACES_PER_FRAME 2
#define MODELS_PER_FRAME 1

// AA Settings
#define MSAA_SAMPLE_AMOUNT 4 // Only used in forward rendering & for water
#define SUPERSAMPLING_FACTOR 1 // 1 means window resolution will be the render resolution

// Texture Filtering Settings
#define ANISOTROPIC_FILTERING_LEVEL 16.0f

// IBL Settings
#define LIGHT_PROBE_RESOLUTION 32
#define REFLECTION_PROBE_MIP_COUNT 5
#define REFLECTION_PROBE_RESOLUTION 128
#define IBL_CAPTURE_RESOLUTION 256 // Should always be greater than the light and reflection probe resolution
#define BRDF_LUT_RESOLUTION 512

// Frustum Options
#define DEFAULT_NEAR_PLANE 0.3f
#define DEFAULT_FAR_PLANE 5000.0f

// Shadowmap Default Options (Resolution controlled by the LightComponent, this will only be used if no light casters are found and a pass needs a fallback)
// Probes do have their own shadow buffers so this should only be used as defaults for LightComponent
#define SHADOWMAP_RESOLUTION_X_DEFAULT 2048
#define SHADOWMAP_RESOLUTION_Y_DEFAULT 2048
#define SHADOWMAP_NEAR_PLANE_DEFAULT 1.0f
#define SHADOWMAP_FAR_PLANE_DEFAULT 200.0f
#define SHADOWMAP_BIAS_DEFAULT 0.007f

// SSAO Options
#define SSAO_KERNEL_SIZE 32 // Maximum amount is restricted by the shader. Only supports a maximum of 64

// Parallax Options
#define PARALLAX_MIN_STEPS 1
#define PARALLAX_MAX_STEPS 20

// Water Options
#define WATER_REFLECTION_NEAR_PLANE_DEFAULT 0.3f
#define WATER_REFLECTION_FAR_PLANE_DEFAULT 1000.0f
#define WATER_REFRACTION_NEAR_PLANE_DEFAULT 0.3f
#define WATER_REFRACTION_FAR_PLANE_DEFAULT 500.0f
