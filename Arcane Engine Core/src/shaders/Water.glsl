#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec4 clipSpace;
out vec2 planeTexCoords;
out vec3 fragToView;

uniform vec3 viewPos;
uniform float waveTiling;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vec4 worldPos = model * vec4(position, 1.0);
	clipSpace = projection * view *worldPos;
	planeTexCoords = texCoords * waveTiling;
	fragToView = viewPos - vec3(worldPos);

	gl_Position = clipSpace;
}




#shader-type fragment
#version 430 core

#define MAX_DIR_LIGHTS 5
#define MAX_POINT_LIGHTS 5
#define MAX_SPOT_LIGHTS 5

struct DirLight {
	vec3 direction;

	float intensity;
	vec3 lightColour;
};

struct PointLight {
	vec3 position;

	float intensity;
	vec3 lightColour;
	float attenuationRadius;
};

struct SpotLight {
	vec3 position;
	vec3 direction;

	float intensity;
	vec3 lightColour;
	float attenuationRadius;

	float cutOff;
	float outerCutOff;
};

in vec4 clipSpace;
in vec2 planeTexCoords;
in vec3 normal;
in vec3 fragToView;

out vec4 FragColour;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvWaveTexture;
uniform sampler2D normalMap;
uniform sampler2D refractionDepthTexture;

// Lighting
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform bool clearWater;
uniform vec3 waterAlbedo;
uniform float albedoPower;
uniform float waveMoveFactor;
uniform vec2 nearFarPlaneValues;

const float waveStrength = 0.02f;
const float shineDamper = 20.0f;
const float reflectivity = 0.6f;
const float waterNormalSmoothing = 1.0f;
const float dampeningEffectStrength = 0.1f;

void main() {
	vec2 ndc = clipSpace.xy / clipSpace.w;
	vec2 textureCoords = ndc * 0.5 + 0.5;

	vec2 reflectCoords = vec2(textureCoords.x, 1.0 - textureCoords.y);
	vec2 refractCoords = vec2(textureCoords.x, textureCoords.y);

	float near = nearFarPlaneValues.x;
	float far = nearFarPlaneValues.y;
	float depth = texture(refractionDepthTexture, refractCoords).r;
	float cameraToSurfaceFloorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	depth = gl_FragCoord.z;
	float cameraToWaterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDepth = cameraToSurfaceFloorDistance - cameraToWaterDistance;
	float dampeningEffect = clamp(waterDepth * dampeningEffectStrength, 0.0, 1.0);
	float dampeningEffect2 = dampeningEffect * dampeningEffect;

	// Apply offset to the sampled coords for the refracted & reflected texture
	vec2 distortion;
	vec2 totalDistortion;
	if (clearWater) {
		distortion = vec2(0.0, 0.0);
		totalDistortion = vec2(0.0, 0.0);
	}
	else {
		distortion = (texture(dudvWaveTexture, vec2(planeTexCoords.x + waveMoveFactor, planeTexCoords.y)).rg * 2.0 - 1.0) * 0.1; // Unpack the dudv map
		distortion = planeTexCoords + vec2(distortion.x, distortion.y + waveMoveFactor);
		totalDistortion = (texture(dudvWaveTexture, distortion).rg * 2.0 - 1.0) * waveStrength * dampeningEffect2;
	}

	reflectCoords += totalDistortion;
	reflectCoords = clamp(reflectCoords, 0.001, 0.999);
	refractCoords += totalDistortion;
	refractCoords = clamp(refractCoords, 0.001, 0.999);
	vec4 reflectedColour = texture(reflectionTexture, reflectCoords);
	vec4 refractedColour = texture(refractionTexture, refractCoords);

	// Calculate other light properties
	vec3 normal;
	if (clearWater) {
		normal = vec3(0.0, 1.0, 0.0);
	}
	else {
		normal = texture(normalMap, distortion).rgb;
		normal = normalize(vec3(normal.r * 2.0 - 1.0, normal.g * waterNormalSmoothing, normal.b * 2.0 - 1.0)); // Assumes the normal of the water plane is always (0, 1, 0) so the the y component for the normal will never be negative
	}
	
	vec3 viewVec = normalize(fragToView);
	float fresnel = dot(viewVec, vec3(0.0, 1.0, 0.0)); // TODO: Should use sampled normal

	// Direct Specular light highlights (TODO: Actually make this work for multiple directional lights, right now it requires there to be one directional light and only uses one)
	vec3 reflectedVec = reflect(normalize(dirLights[0].direction), normal);
	float specular = max(dot(reflectedVec, viewVec), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specHighlight = dirLights[0].lightColour * specular * reflectivity * dampeningEffect2;

	// Finally combine results for the pixel
	FragColour = mix(reflectedColour, refractedColour, fresnel); // Should be fresnel value
	FragColour = mix(FragColour, vec4(waterAlbedo, 1.0), albedoPower) + vec4(specHighlight, 0.0);
	FragColour.a = dampeningEffect;
	//FragColour = vec4(dampeningEffect, dampeningEffect, dampeningEffect, 1.0);
}
