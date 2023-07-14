#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec3 worldFragPos;
out vec4 clipSpace;
out vec2 planeTexCoords;
out vec3 fragToView;

uniform vec3 viewPos;
uniform vec2 waveTiling;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	worldFragPos = vec3(model * vec4(position, 1.0));
	clipSpace = projection * view * vec4(worldFragPos, 1.0);
	planeTexCoords = vec2(texCoords.x * waveTiling.x, texCoords.y * waveTiling.y);
	fragToView = viewPos - worldFragPos;

	gl_Position = clipSpace;
}




#shader-type fragment
#version 430 core

#define MAX_DIR_LIGHTS 3
#define MAX_POINT_LIGHTS 6
#define MAX_SPOT_LIGHTS 6

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

in vec3 worldFragPos;
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
uniform ivec4 numDirPointSpotLights;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform mat4 viewInverse;
uniform mat4 projectionInverse;

uniform bool hasReflectionRefraction;
uniform bool clearWater;
uniform bool shouldShine;
uniform vec3 waterAlbedo;
uniform float albedoPower;
uniform float waveMoveFactor;
uniform float waveStrength;
uniform float shineDamper;
uniform float waterNormalSmoothing;
uniform float depthDampeningEffect;

// Function Declarations
vec3 WorldPosFromDepth(vec2 texCoords);

void main() {
	vec2 ndc = clipSpace.xy / clipSpace.w;
	vec2 textureCoords = ndc * 0.5 + 0.5;

	vec2 reflectCoords = vec2(textureCoords.x, 1.0 - textureCoords.y);
	vec2 refractCoords = vec2(textureCoords.x, textureCoords.y);

	// Apply offset to the sampled coords for the refracted & reflected texture
	vec2 distortion;
	vec2 totalDistortion;
	float dampeningEffect2 = 1.0f; // Default to 1 in-case we do not have refraction for depth dampening
	if (clearWater) {
		distortion = vec2(0.0, 0.0);
		totalDistortion = vec2(0.0, 0.0);
	}
	else {
		if (hasReflectionRefraction) {
			vec3 refractedSurface = WorldPosFromDepth(refractCoords);
			float waterDepthAtRefractedSurface = worldFragPos.y - refractedSurface.y;
			float dampeningEffect = clamp(waterDepthAtRefractedSurface * depthDampeningEffect, 0.0, 1.0);
			dampeningEffect2 = dampeningEffect * dampeningEffect;
		}

		distortion = (texture(dudvWaveTexture, vec2(planeTexCoords.x + waveMoveFactor, planeTexCoords.y)).rg * 2.0 - 1.0) * 0.1; // Unpack the dudv map
		distortion = planeTexCoords + vec2(distortion.x, distortion.y + waveMoveFactor);
		totalDistortion = (texture(dudvWaveTexture, distortion).rg * 2.0 - 1.0) * waveStrength * dampeningEffect2;
	}

	vec4 reflectedColour = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 refractedColour = vec4(1.0, 1.0, 1.0, 1.0);
	if (hasReflectionRefraction) {
		reflectCoords += totalDistortion;
		reflectCoords = clamp(reflectCoords, 0.001, 0.999);
		refractCoords += totalDistortion;
		refractCoords = clamp(refractCoords, 0.001, 0.999);
		reflectedColour = texture(reflectionTexture, reflectCoords);
		refractedColour = texture(refractionTexture, refractCoords);
	}

	// Calculate other light properties
	vec3 normal;
	if (clearWater) {
		normal = vec3(0.0, 1.0, 0.0);
	}
	else {
		normal = texture(normalMap, distortion).rgb;
		normal = normalize(vec3(normal.r * 2.0 - 1.0, normal.b * waterNormalSmoothing, normal.g * 2.0 - 1.0)); // Normal maps are in tangent space but we know tangent space z component = y component in world space since plane is always flat and assume normal is (0, 1, 0)
	}
	
	vec3 viewVec = normalize(fragToView);
	float fresnel = dot(viewVec, vec3(0.0, 1.0, 0.0)); // TODO: Should use sampled normal

	// Specular (reflection) light highlights
	vec3 specHighlight = vec3(0.0, 0.0, 0.0);
	if (shouldShine) {
		// Directional light specular contribution
		for (int i = 0; i < numDirPointSpotLights.x; i++) {
			vec3 reflectedVec = reflect(normalize(dirLights[i].direction), normal);
			float specular = pow(max(dot(reflectedVec, viewVec), 0.0), shineDamper);
			specHighlight += dirLights[i].intensity * dirLights[i].lightColour * specular * dampeningEffect2;
		}

		// Point light specular contribution
		for (int i = 0; i < numDirPointSpotLights.y; i++) {
			vec3 lightToFrag = worldFragPos - pointLights[i].position;
			vec3 reflectedVec = reflect(normalize(lightToFrag), normal);
			float specular = pow(max(dot(reflectedVec, viewVec), 0.0), shineDamper);

			// Attenuation calculation (based on Epic's UE4 falloff model)
			float lightToFragDistance = length(lightToFrag);
			float d = lightToFragDistance / pointLights[i].attenuationRadius;
			float d2 = d * d;
			float d4 = d2 * d2;
			float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);
			float attenuation = (falloffNumerator * falloffNumerator) / ((lightToFragDistance * lightToFragDistance) + 1.0);

			specHighlight += pointLights[i].intensity * pointLights[i].lightColour * specular * attenuation * dampeningEffect2;
		}

		// Spot light specular contribution
		for (int i = 0; i < numDirPointSpotLights.z; i++) {
			vec3 lightToFrag = worldFragPos - spotLights[i].position;
			vec3 lightToFragNorm = normalize(lightToFrag);
			vec3 reflectedVec = reflect(lightToFragNorm, normal);
			float specular = pow(max(dot(reflectedVec, viewVec), 0.0), shineDamper);

			// Attenuation calculation (based on Epic's UE4 falloff model)
			float lightToFragDistance = length(lightToFrag);
			float d = lightToFragDistance / spotLights[i].attenuationRadius;
			float d2 = d * d;
			float d4 = d2 * d2;
			float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);

			// Check if it is in the spotlight's circle and calculate final attenuation
			float theta = dot(normalize(spotLights[i].direction), lightToFragNorm);
			float difference = spotLights[i].cutOff - spotLights[i].outerCutOff;
			float intensity = clamp((theta - spotLights[i].outerCutOff) / difference, 0.0, 1.0);
			float attenuation = intensity * (falloffNumerator * falloffNumerator) / ((lightToFragDistance * lightToFragDistance) + 1.0);

			specHighlight += spotLights[i].intensity * spotLights[i].lightColour * specular * attenuation * dampeningEffect2;
		}
	}

	// Finally combine results for the pixel
	FragColour = mix(reflectedColour, refractedColour, fresnel);
	FragColour = mix(FragColour, vec4(waterAlbedo, 1.0), albedoPower) + vec4(specHighlight, 0.0);
}

vec3 WorldPosFromDepth(vec2 texCoords) {
	float z = 2.0 * texture(refractionDepthTexture, texCoords).r - 1.0; // [-1, 1]
	vec4 clipSpacePos = vec4(texCoords * 2.0 - 1.0 , z, 1.0);
	vec4 viewSpacePos = projectionInverse * clipSpacePos;
	viewSpacePos /= viewSpacePos.w; // Perspective division

	vec4 worldSpacePos = viewInverse * viewSpacePos;

	return worldSpacePos.xyz;
}
