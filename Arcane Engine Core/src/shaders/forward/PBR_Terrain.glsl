#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out mat3 TBN;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	// Use the normal matrix to maintain the orthogonal property of a vector when it is scaled non-uniformly
	vec3 T = normalize(normalMatrix * tangent);
	vec3 B = normalize(normalMatrix * bitangent);
	vec3 N = normalize(normalMatrix * normal);
	TBN = mat3(T, B, N);

	FragPos = vec3(model * vec4(position, 1.0f));
	TexCoords = texCoords;

	gl_Position = projection * view * vec4(FragPos, 1.0);
}




#shader-type fragment
#version 430 core

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D texture_albedo1; // background texture
	sampler2D texture_albedo2; // r texture
	sampler2D texture_albedo3; // g texture
	sampler2D texture_albedo4; // b texture

	sampler2D texture_normal1; // background texture
	sampler2D texture_normal2; // r texture
	sampler2D texture_normal3; // g texture
	sampler2D texture_normal4; // b texture

	sampler2D texture_roughness1; // background texture
	sampler2D texture_roughness2; // r texture
	sampler2D texture_roughness3; // g texture
	sampler2D texture_roughness4; // b texture

	sampler2D texture_metallic1; // background texture
	sampler2D texture_metallic2; // r texture
	sampler2D texture_metallic3; // g texture
	sampler2D texture_metallic4; // b texture

	sampler2D texture_AO1; // background texture
	sampler2D texture_AO2; // r texture
	sampler2D texture_AO3; // g texture
	sampler2D texture_AO4; // b texture

	sampler2D blendmap;
	float tilingAmount;
};

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

#define MAX_DIR_LIGHTS 5
#define MAX_POINT_LIGHTS 5
#define MAX_SPOT_LIGHTS 5
const float PI = 3.14159265359;

in mat3 TBN;
in vec2 TexCoords;
in vec3 FragPos;

out vec4 color;

uniform sampler2D shadowmap;
uniform ivec4 numDirPointSpotLights;
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform Material material;
uniform vec3 viewPos;
uniform mat4 lightSpaceViewProjectionMatrix;

// Light radiance calculations
vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);
vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);
vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity);

// Cook-Torrance BRDF functions adopted by Epic for UE4
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
float GeometrySchlickGGX(float cosTheta, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 baseReflectivity);

// Other function prototypes
vec3 UnpackNormal(vec3 textureNormal);
float CalculateShadow(vec3 normal, vec3 fragToLight);

void main() {
	vec4 blendMapColour = texture(material.blendmap, TexCoords);
	float backTextureWeight = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec2 tiledCoords = TexCoords * material.tilingAmount;

	vec3 backgroundTextureAlbedo = texture(material.texture_albedo1, tiledCoords).rgb * backTextureWeight;
	vec3 rTextureAlbedo = texture(material.texture_albedo2, tiledCoords).rgb * blendMapColour.r;
	vec3 gTextureAlbedo = texture(material.texture_albedo3, tiledCoords).rgb * blendMapColour.g;
	vec3 bTextureAlbedo = texture(material.texture_albedo4, tiledCoords).rgb * blendMapColour.b;
	vec3 albedo = backgroundTextureAlbedo + rTextureAlbedo + gTextureAlbedo + bTextureAlbedo;

	vec3 backgroundTextureNormal = UnpackNormal(texture(material.texture_normal1, tiledCoords).rgb) * backTextureWeight;
	vec3 rTextureNormal = UnpackNormal(texture(material.texture_normal2, tiledCoords).rgb) * blendMapColour.r;
	vec3 gTextureNormal = UnpackNormal(texture(material.texture_normal3, tiledCoords).rgb) * blendMapColour.g;
	vec3 bTextureNormal = UnpackNormal(texture(material.texture_normal4, tiledCoords).rgb) * blendMapColour.b;
	vec3 normal = normalize(backgroundTextureNormal + rTextureNormal + gTextureNormal + bTextureNormal);

	float backgroundTextureRoughness = texture(material.texture_roughness1, tiledCoords).r * backTextureWeight;
	float rTextureRoughness = texture(material.texture_roughness2, tiledCoords).r * blendMapColour.r;
	float gTextureRoughness = texture(material.texture_roughness3, tiledCoords).r * blendMapColour.g;
	float bTextureRoughness = texture(material.texture_roughness4, tiledCoords).r * blendMapColour.b;
	float roughness = max(max(backgroundTextureRoughness, rTextureRoughness), max(gTextureRoughness, bTextureRoughness));
	roughness = max(roughness, 0.04); // Used for calculations since specular highlights will be too fine, and will cause flicker

	float backgroundTextureMetallic = texture(material.texture_metallic1, tiledCoords).r * backTextureWeight;
	float rTextureMetallic = texture(material.texture_metallic2, tiledCoords).r * blendMapColour.r;
	float gTextureMetallic = texture(material.texture_metallic3, tiledCoords).r * blendMapColour.g;
	float bTextureMetallic = texture(material.texture_metallic4, tiledCoords).r * blendMapColour.b;
	float metallic = max(max(backgroundTextureMetallic, rTextureMetallic), max(gTextureMetallic, bTextureMetallic));

	float backgroundTextureAO = texture(material.texture_AO1, tiledCoords).r * backTextureWeight;
	float rTextureAO = texture(material.texture_AO2, tiledCoords).r * blendMapColour.r;
	float gTextureAO = texture(material.texture_AO3, tiledCoords).r * blendMapColour.g;
	float bTextureAO = texture(material.texture_AO4, tiledCoords).r * blendMapColour.b;
	float ao = max(max(backgroundTextureAO, rTextureAO), max(gTextureAO, bTextureAO));

	// Normal mapping code. Opted out of tangent space normal mapping since I would have to convert all of my lights to tangent space
	normal = normalize(TBN * UnpackNormal(normal));

	vec3 fragToView = normalize(viewPos - FragPos);

	// Dielectrics have an average base specular reflectivity around 0.04, and metals absorb all of their diffuse (refraction) lighting so their albedo is used instead for their specular lighting (reflection)
	vec3 baseReflectivity = vec3(0.04);
	baseReflectivity = mix(baseReflectivity, albedo, metallic);

	// Calculate per light radiance for all of the direct lighting
	vec3 directLightIrradiance = vec3(0.0);
	directLightIrradiance += CalculateDirectionalLightRadiance(albedo, normal, metallic, roughness, fragToView, baseReflectivity);
	directLightIrradiance += CalculatePointLightRadiance(albedo, normal, metallic, roughness, fragToView, baseReflectivity);
	directLightIrradiance += CalculateSpotLightRadiance(albedo, normal, metallic, roughness, fragToView, baseReflectivity);

	// Calculate ambient term
	vec3 ambient = vec3(0.05) * albedo * ao;
	
	// Result
	color = vec4(ambient + directLightIrradiance, 1.0);
}

// TODO: Need to also add multiple shadow support
vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 directLightIrradiance = vec3(0.0);

	for (int i = 0; i < numDirPointSpotLights.x; ++i) {
		vec3 lightDir = normalize(-dirLights[i].direction);
		vec3 halfway = normalize(lightDir + fragToView);
		vec3 radiance = dirLights[i].intensity * dirLights[i].lightColour;

		// Cook-Torrance Specular BRDF calculations
		float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
		vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
		float geometry = GeometrySmith(normal, fragToView, lightDir, roughness);

		// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
		vec3 specularRatio = fresnel;
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		// Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
		vec3 numerator = specularRatio * normalDistribution * geometry;
		float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(lightDir, normal), 0.0) + 0.001;  // Prevents any division by zero
		vec3 specular = numerator / denominator;

		// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
		vec3 diffuse = diffuseRatio * albedo / PI;

		// Add the light's radiance to the irradiance sum
		directLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, lightDir), 0.0) * (1.0 - CalculateShadow(normal, lightDir));
	}

	return directLightIrradiance;
}


vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 pointLightIrradiance = vec3(0.0);

	for (int i = 0; i < numDirPointSpotLights.y; ++i) {
		vec3 fragToLight = normalize(pointLights[i].position - FragPos);
		vec3 halfway = normalize(fragToView + fragToLight);
		float fragToLightDistance = length(pointLights[i].position - FragPos);

		// Attenuation calculation (based on Epic's UE4 falloff model)
		float d = fragToLightDistance / pointLights[i].attenuationRadius;
		float d2 = d * d;
		float d4 = d2 * d2;
		float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);
		float attenuation = (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
		vec3 radiance = pointLights[i].intensity * pointLights[i].lightColour * attenuation;

		// Cook-Torrance Specular BRDF calculations
		float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
		vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
		float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

		// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
		vec3 specularRatio = fresnel;
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		// Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
		vec3 numerator = specularRatio * normalDistribution * geometry;
		float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(fragToLight, normal), 0.0) + 0.001; // Prevents any division by zero
		vec3 specular = numerator / denominator;

		// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
		vec3 diffuse = diffuseRatio * albedo / PI;

		// Add the light's radiance to the irradiance sum
		pointLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
	}

	return pointLightIrradiance;
}


vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 spotLightIrradiance = vec3(0.0);

	for (int i = 0; i < numDirPointSpotLights.z; ++i) {
		vec3 fragToLight = normalize(spotLights[i].position - FragPos);
		vec3 halfway = normalize(fragToView + fragToLight);
		float fragToLightDistance = length(spotLights[i].position - FragPos);

		// Attenuation calculation (based on Epic's UE4 falloff model)
		float d = fragToLightDistance / spotLights[i].attenuationRadius;
		float d2 = d * d;
		float d4 = d2 * d2;
		float falloffNumerator = clamp(1.0 - d4, 0.0, 1.0);

		// Check if it is in the spotlight's circle
		float theta = dot(normalize(spotLights[i].direction), -fragToLight);
		float difference = spotLights[i].cutOff - spotLights[i].outerCutOff;
		float intensity = clamp((theta - spotLights[i].outerCutOff) / difference, 0.0, 1.0);
		float attenuation = intensity * (falloffNumerator * falloffNumerator) / ((fragToLightDistance * fragToLightDistance) + 1.0);
		vec3 radiance = spotLights[i].intensity * spotLights[i].lightColour * attenuation;

		// Cook-Torrance Specular BRDF calculations
		float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
		vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
		float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

		// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
		vec3 specularRatio = fresnel;
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		// Finally calculate the specular part of the Cook-Torrance BRDF (max 0.1 stops any visual artifacts)
		vec3 numerator = specularRatio * normalDistribution * geometry;
		float denominator = 4 * max(dot(fragToView, normal), 0.1) * max(dot(fragToLight, normal), 0.0) + 0.001; // Prevents any division by zero
		vec3 specular = numerator / denominator;

		// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
		vec3 diffuse = diffuseRatio * albedo / PI;

		// Add the light's radiance to the irradiance sum
		spotLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
	}

	return spotLightIrradiance;
}


// Approximates the amount of microfacets that are properly aligned with the halfway vector, thus determines the strength and area for specular light
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float normDotHalf = dot(normal, halfway);
	float normDotHalf2 = normDotHalf * normDotHalf;

	float numerator = a2;
	float denominator = normDotHalf2 * (a2 - 1.0) + 1.0;
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}


// Approximates the geometry obstruction and geometry shadowing respectively, on the microfacet level
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
	return GeometrySchlickGGX(max(dot(normal, viewDir), 0.0), roughness) * GeometrySchlickGGX(max(dot(normal, lightDir), 0.0), roughness);
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


// Unpacks the normal from the texture and returns the normal in tangent space
vec3 UnpackNormal(vec3 textureNormal) {
	return normalize(textureNormal * 2.0 - 1.0);
}


float CalculateShadow(vec3 normal, vec3 fragToLight) {
	vec4 fragPosLightClipSpace = lightSpaceViewProjectionMatrix * vec4(FragPos, 1.0);
	vec3 ndcCoords = fragPosLightClipSpace.xyz / fragPosLightClipSpace.w;
	vec3 depthmapCoords = ndcCoords * 0.5 + 0.5;

	float shadow = 0.0;
	float currentDepth = depthmapCoords.z;

	// Add shadow bias to avoid shadow acne. However too much bias can cause peter panning
	float shadowBias = 0.003;

	// Perform Percentage Closer Filtering (PCF) in order to produce soft shadows
	vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			float sampledDepthPCF = texture(shadowmap, depthmapCoords.xy + (texelSize * vec2(x, y))).r;
			shadow += currentDepth > sampledDepthPCF + shadowBias ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (currentDepth > 1.0)
		shadow = 0.0;
	return shadow;
}
