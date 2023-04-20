#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoords;

void main() {
	gl_Position = vec4(position, 1.0);
	TexCoords = texCoord;
}




#shader-type fragment
#version 430 core

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

struct ShadowData {
	mat4 lightSpaceViewProjectionMatrix;
	bool hasShadow;
	float shadowBias;
	int lightShadowIndex;
};

struct ShadowDataPointLight {
	float farPlane;
	bool hasShadow;
	float shadowBias;
	int lightShadowIndex;
};

#define MAX_DIR_LIGHTS 3
#define MAX_POINT_LIGHTS 6
#define MAX_SPOT_LIGHTS 6
const float PI = 3.14159265359;

in vec2 TexCoords;

out vec4 color;

// GBuffer
uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D materialInfoTexture;
uniform sampler2D ssaoTexture;
uniform sampler2D depthTexture;

// IBL
uniform int reflectionProbeMipCount;
uniform bool computeIBL;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Lighting
uniform ivec4 numDirPointSpotLights;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 viewPos;
uniform mat4 viewInverse;
uniform mat4 projectionInverse;

// Shadow Data
uniform sampler2D dirLightShadowmap;
uniform ShadowData dirLightShadowData;
uniform sampler2D spotLightShadowmap;
uniform ShadowData spotLightShadowData;
uniform samplerCube pointLightShadowCubemap;
uniform ShadowDataPointLight pointLightShadowData;

// Light radiance calculations
vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToViewNorm, vec3 baseReflectivity);
vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToViewNorm, vec3 baseReflectivity);
vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToViewNorm, vec3 baseReflectivity);

// Cook-Torrance BRDF functions adopted by Epic for UE4
float NormalDistributionGGX(vec3 normal, vec3 halfwayNorm, float roughness);
float GeometrySmith(vec3 normal, vec3 fragToViewNorm, vec3 lightDirNorm, float roughness);
float GeometrySchlickGGX(float cosTheta, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 baseReflectivity);

// Other function prototypes
float CalculateDirLightShadow(vec3 fragPos);
float CalculateSpotLightShadow(vec3 fragPos);
float CalculatePointLightShadow(vec3 lightToFrag);
vec3 WorldPosFromDepth();

void main() {
	// Sample textures
	vec4 sampledAlbedo = texture(albedoTexture, TexCoords).rgba;
	vec3 albedo = sampledAlbedo.rgb;
	float albedoAlpha = sampledAlbedo.w;
	vec3 normal = texture(normalTexture, TexCoords).rgb;
	float metallic = texture(materialInfoTexture, TexCoords).r;
	float unclampedRoughness = texture(materialInfoTexture, TexCoords).g; // Used for indirect specular (reflections)
	float roughness = max(unclampedRoughness, 0.04); // Used for calculations since specular highlights will be too fine, and will cause flicker
	float materialAO = texture(materialInfoTexture, TexCoords).b;
	float sceneAO = texture(ssaoTexture, TexCoords).r;
	float ao = min(materialAO, sceneAO);

	// Reconstruct fragPos
	vec3 fragPos = WorldPosFromDepth();

	vec3 fragToViewNorm = normalize(viewPos - fragPos);
	vec3 reflectionVec = reflect(-fragToViewNorm, normal);

	// Dielectrics have an average base specular reflectivity around 0.04, and metals absorb all of their diffuse (refraction) lighting so their albedo is used instead for their specular lighting (reflection)
	vec3 baseReflectivity = vec3(0.04);
	baseReflectivity = mix(baseReflectivity, albedo, metallic);

	// Calculate per light radiance for all of the direct lighting
	vec3 directLightIrradiance = vec3(0.0);
	directLightIrradiance += CalculateDirectionalLightRadiance(albedo, normal, metallic, roughness, fragPos, fragToViewNorm, baseReflectivity);
	directLightIrradiance += CalculatePointLightRadiance(albedo, normal, metallic, roughness, fragPos, fragToViewNorm, baseReflectivity);
	directLightIrradiance += CalculateSpotLightRadiance(albedo, normal, metallic, roughness, fragPos, fragToViewNorm, baseReflectivity);

	// Calcualte ambient IBL for both diffuse and specular
	vec3 ambient = vec3(0.05) * albedo * ao;
	if (computeIBL) {
		vec3 specularRatio = FresnelSchlick(max(dot(normal, fragToViewNorm), 0.0), baseReflectivity);
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		vec3 indirectDiffuse = texture(irradianceMap, normal).rgb * albedo * diffuseRatio;

		vec3 prefilterColour = textureLod(prefilterMap, reflectionVec, unclampedRoughness * (reflectionProbeMipCount - 1)).rgb;
		vec2 brdfIntegration = texture(brdfLUT, vec2(max(dot(normal, fragToViewNorm), 0.0), roughness)).rg;
		vec3 indirectSpecular = prefilterColour * (specularRatio * brdfIntegration.x + brdfIntegration.y);

		ambient = (indirectDiffuse + indirectSpecular) * ao;
	}

	color = vec4(ambient + directLightIrradiance, 1.0);
}

vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToViewNorm, vec3 baseReflectivity) {
	vec3 directLightIrradiance = vec3(0.0);

	for (int i = 0; i < numDirPointSpotLights.x; ++i) {
		vec3 lightDirNorm = normalize(-dirLights[i].direction);
		vec3 halfwayNorm = normalize(lightDirNorm + fragToViewNorm);
		vec3 radiance = dirLights[i].intensity * dirLights[i].lightColour;

		// Cook-Torrance Specular BRDF calculations
		float normalDistribution = NormalDistributionGGX(normal, halfwayNorm, roughness);
		vec3 fresnel = FresnelSchlick(max(dot(halfwayNorm, fragToViewNorm), 0.0), baseReflectivity);
		float geometry = GeometrySmith(normal, fragToViewNorm, lightDirNorm, roughness);

		// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
		vec3 specularRatio = fresnel;
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		// Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
		vec3 numerator = specularRatio * normalDistribution * geometry;
		float denominator = 4 * max(dot(fragToViewNorm, normal), 0.1) * max(dot(lightDirNorm, normal), 0.0) + 0.001;  // Prevents any division by zero
		vec3 specular = numerator / denominator;

		// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
		vec3 diffuse = diffuseRatio * albedo / PI;

		// Calculate shadows, but first check to make sure the current light index is the shadow caster
		float shadowAmount = 0.0f;
		if (i == dirLightShadowData.lightShadowIndex)
			shadowAmount = CalculateDirLightShadow(fragPos);

		// Add the light's radiance to the irradiance sum
		directLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, lightDirNorm), 0.0) * (1.0 - shadowAmount);
	}

	return directLightIrradiance;
}


vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragPos, vec3 fragToViewNorm, vec3 baseReflectivity) {
	vec3 pointLightIrradiance = vec3(0.0);

	for (int i = 0; i < numDirPointSpotLights.y; ++i) {
		vec3 fragToLightNorm = normalize(pointLights[i].position - fragPos);
		vec3 halfwayNorm = normalize(fragToViewNorm + fragToLightNorm);
		float fragToLightDistance = length(pointLights[i].position - fragPos);

		// Attenuation calculation (based on Epic's UE4 falloff model)
		float d = fragToLightDistance / pointLights[i].attenuationRadius;
		float d2 = d * d;
		float d4 = d2 * d2;
		float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);
		float attenuation = (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
		vec3 radiance = pointLights[i].intensity * pointLights[i].lightColour * attenuation;

		// Cook-Torrance Specular BRDF calculations
		float normalDistribution = NormalDistributionGGX(normal, halfwayNorm, roughness);
		vec3 fresnel = FresnelSchlick(max(dot(halfwayNorm, fragToViewNorm), 0.0), baseReflectivity);
		float geometry = GeometrySmith(normal, fragToViewNorm, fragToLightNorm, roughness);

		// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
		vec3 specularRatio = fresnel;
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		// Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
		vec3 numerator = specularRatio * normalDistribution * geometry;
		float denominator = 4 * max(dot(fragToViewNorm, normal), 0.1) * max(dot(fragToLightNorm, normal), 0.0) + 0.001; // Prevents any division by zero
		vec3 specular = numerator / denominator;

		// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
		vec3 diffuse = diffuseRatio * albedo / PI;

		// Calculate shadows, but first check to make sure the current light index is the shadow caster
		float shadowAmount = 0.0;
		if (i == pointLightShadowData.lightShadowIndex)
			shadowAmount = CalculatePointLightShadow(-fragToLightNorm);

		// Add the light's radiance to the irradiance sum
		pointLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, fragToLightNorm), 0.0) * (1.0 - shadowAmount);
	}

	return pointLightIrradiance;
}


vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness,  vec3 fragPos, vec3 fragToViewNorm, vec3 baseReflectivity) {
	vec3 spotLightIrradiance = vec3(0.0);

	for (int i = 0; i < numDirPointSpotLights.z; ++i) {
		vec3 fragToLightNorm = normalize(spotLights[i].position - fragPos);
		vec3 halfwayNorm = normalize(fragToViewNorm + fragToLightNorm);
		float fragToLightDistance = length(spotLights[i].position - fragPos);

		// Attenuation calculation (based on Epic's UE4 falloff model)
		float d = fragToLightDistance / spotLights[i].attenuationRadius;
		float d2 = d * d;
		float d4 = d2 * d2;
		float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);

		// Check if it is in the spotlight's circle
		float theta = dot(normalize(spotLights[i].direction), -fragToLightNorm);
		float difference = spotLights[i].cutOff - spotLights[i].outerCutOff;
		float intensity = clamp((theta - spotLights[i].outerCutOff) / difference, 0.0, 1.0);
		float attenuation = intensity * (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
		vec3 radiance = spotLights[i].intensity * spotLights[i].lightColour * attenuation;

		// Cook-Torrance Specular BRDF calculations
		float normalDistribution = NormalDistributionGGX(normal, halfwayNorm, roughness);
		vec3 fresnel = FresnelSchlick(max(dot(halfwayNorm, fragToViewNorm), 0.0), baseReflectivity);
		float geometry = GeometrySmith(normal, fragToViewNorm, fragToLightNorm, roughness);

		// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
		vec3 specularRatio = fresnel;
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		// Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
		vec3 numerator = specularRatio * normalDistribution * geometry;
		float denominator = 4 * max(dot(fragToViewNorm, normal), 0.1) * max(dot(fragToLightNorm, normal), 0.0) + 0.001; // Prevents any division by zero
		vec3 specular = numerator / denominator;

		// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
		vec3 diffuse = diffuseRatio * albedo / PI;

		// Calculate shadows, but first check to make sure the current light index is the shadow caster
		float shadowAmount = 0.0f;
		if (i == spotLightShadowData.lightShadowIndex)
			shadowAmount = CalculateSpotLightShadow(fragPos);

		// Add the light's radiance to the irradiance sum
		spotLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, fragToLightNorm), 0.0) * (1.0 - shadowAmount);
	}

	return spotLightIrradiance;
}


// Approximates the amount of microfacets that are properly aligned with the halfway vector, thus determines the strength and area for specular light
float NormalDistributionGGX(vec3 normal, vec3 halfwayNorm, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float normDotHalf = dot(normal, halfwayNorm);
	float normDotHalf2 = normDotHalf * normDotHalf;

	float numerator = a2;
	float denominator = normDotHalf2 * (a2 - 1.0) + 1.0;
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}


// Approximates the geometry obstruction and geometry shadowing respectively, on the microfacet level
float GeometrySmith(vec3 normal, vec3 fragToViewNorm, vec3 lightDirNorm, float roughness) {
	return GeometrySchlickGGX(max(dot(normal, fragToViewNorm), 0.0), roughness) * GeometrySchlickGGX(max(dot(normal, lightDirNorm), 0.0), roughness);
}
float GeometrySchlickGGX(float cosTheta, float roughness) {
	float r = (roughness + 1.0);
	float k = (roughness * roughness) / 8.0;

	float numerator = cosTheta;
	float denominator = cosTheta * (1.0 - k) + k;

	return numerator / max(denominator, 0.001);
}


// Calculates the amount of specular light. Since diffuse(refraction) and specular(reflection) are mutually exclusive, 
// we can also use this to determine the amount of diffuse light
// Taken from UE4's implementation which is faster and basically identical to the usual Fresnel calculations: https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
vec3 FresnelSchlick(float cosTheta, vec3 baseReflectivity) {
	return max(baseReflectivity + (1.0 - baseReflectivity) * pow(2, (-5.55473 * cosTheta - 6.98316) * cosTheta), 0.0);
}


float CalculateDirLightShadow(vec3 fragPos) {
	if (!dirLightShadowData.hasShadow)
		return 0.0;

	vec4 fragPosLightClipSpace = dirLightShadowData.lightSpaceViewProjectionMatrix * vec4(fragPos, 1.0);
	vec3 ndcCoords = fragPosLightClipSpace.xyz / fragPosLightClipSpace.w;
	vec3 depthmapCoords = ndcCoords * 0.5 + 0.5;

	float shadow = 0.0;
	float currentDepth = depthmapCoords.z;

	// Perform Percentage Closer Filtering (PCF) in order to produce soft shadows
	vec2 texelSize = 1.0 / textureSize(dirLightShadowmap, 0);
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			float sampledDepthPCF = texture(dirLightShadowmap, depthmapCoords.xy + (texelSize * vec2(x, y))).r;
			shadow += currentDepth > sampledDepthPCF + dirLightShadowData.shadowBias ? 1.0 : 0.0; // Add shadow bias to avoid shadow acne. However too much bias can cause peter panning
		}
	}
	shadow /= 9.0;

	if (currentDepth > 1.0)
		shadow = 0.0;
	return shadow;
}

float CalculateSpotLightShadow(vec3 fragPos) {
	if (!spotLightShadowData.hasShadow)
		return 0.0;

	vec4 fragPosLightClipSpace = spotLightShadowData.lightSpaceViewProjectionMatrix * vec4(fragPos, 1.0);
	vec3 ndcCoords = fragPosLightClipSpace.xyz / fragPosLightClipSpace.w;
	vec3 depthmapCoords = ndcCoords * 0.5 + 0.5;

	float shadow = 0.0;
	float currentDepth = depthmapCoords.z;

	// Perform Percentage Closer Filtering (PCF) in order to produce soft shadows
	vec2 texelSize = 1.0 / textureSize(spotLightShadowmap, 0);
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			float sampledDepthPCF = texture(spotLightShadowmap, depthmapCoords.xy + (texelSize * vec2(x, y))).r;
			shadow += currentDepth > sampledDepthPCF + spotLightShadowData.shadowBias ? 1.0 : 0.0; // Add shadow bias to avoid shadow acne. However too much bias can cause peter panning
		}
	}
	shadow /= 9.0;

	if (currentDepth > 1.0)
		shadow = 0.0;
	return shadow;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float CalculatePointLightShadow(vec3 lightToFrag) {
	if (!pointLightShadowData.hasShadow)
		return 0.0;

	float currentDepth = length(lightToFrag);

	float shadow = 0.0;
	float samples = 20;
	float diskRadius = 0.05;
	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(pointLightShadowCubemap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= pointLightShadowData.farPlane; // undo the [0,1] mapping
		if (currentDepth - pointLightShadowData.shadowBias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

	return shadow;
}

vec3 WorldPosFromDepth() {
	float z = 2.0 * texture(depthTexture, TexCoords).r - 1.0; // [-1, 1]
	vec4 clipSpacePos = vec4(TexCoords * 2.0 - 1.0 , z, 1.0);
	vec4 viewSpacePos = projectionInverse * clipSpacePos;
	viewSpacePos /= viewSpacePos.w; // Perspective division

	vec4 worldSpacePos = viewInverse * viewSpacePos;

	return worldSpacePos.xyz;
}
