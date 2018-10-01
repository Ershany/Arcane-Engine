#version 430 core

struct Material {
	sampler2D texture_albedo;
	sampler2D texture_normal;
	sampler2D texture_metallic;
	sampler2D texture_roughness;
	sampler2D texture_ao;
};

struct DirLight {
	vec3 direction;

	vec3 lightColour; // radiant flux
};

struct PointLight {
	vec3 position;

	vec3 lightColour; // radiant flux
};

struct SpotLight {
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	vec3 lightColour; // radiant flux
};

#define MAX_POINT_LIGHTS 5
const float PI = 3.14159265359;

in mat3 TBN;
in vec3 FragPos;
in vec4 FragPosLightClipSpace;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform sampler2D shadowmap;
uniform int numPointLights;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;


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
float CalculateShadow(vec3 normal, vec3 fragToDirLight);

void main() {
	// Sample textures
	vec3 albedo = texture(material.texture_albedo, TexCoords).rgb;
	vec3 normal = texture(material.texture_normal, TexCoords).rgb;
	float metallic = texture(material.texture_metallic, TexCoords).r;
	float roughness = max(texture(material.texture_roughness, TexCoords).r, 0.04);
	float ao = texture(material.texture_ao, TexCoords).r;

	// Normal mapping code. Opted out of tangent space normal mapping since I would have to convert all of my lights to tangent space
	normal = normalize(normal * 2.0f - 1.0f);
	normal = normalize(TBN * normal);
	
	vec3 fragToView = normalize(viewPos - FragPos);

	// Dielectrics have an average base specular reflectivity around 0.04, and metals absorb all of their diffuse (refraction) lighting so their albedo is used instead for their specular lighting (reflection)
	vec3 baseReflectivity = vec3(0.04);
	baseReflectivity = mix(baseReflectivity, albedo, metallic);

	// Calculate per light radiance for all of the direct lighting
	vec3 directLightIrradiance = vec3(0.0);
	directLightIrradiance += CalculateDirectionalLightRadiance(albedo, normal, metallic, roughness, fragToView, baseReflectivity);
	directLightIrradiance += CalculatePointLightRadiance(albedo, normal, metallic, roughness, fragToView, baseReflectivity);
	directLightIrradiance += CalculateSpotLightRadiance(albedo, normal, metallic, roughness, fragToView, baseReflectivity);

	// Calcualte ambient IBL for both diffuse and specular
	vec3 specularRatio = FresnelSchlick(max(dot(normal, fragToView), 0.0), baseReflectivity);
	vec3 diffuseRatio = vec3(1.0) - specularRatio;
	diffuseRatio *= 1.0 - metallic;
	vec3 indirectIrradiance = texture(irradianceMap, normal).rgb;
	vec3 indirectDiffuse = indirectIrradiance * albedo;
	vec3 ambient = (diffuseRatio * indirectDiffuse) * ao;

	color = vec4(ambient + directLightIrradiance, 1.0);
}


vec3 CalculateDirectionalLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 lightDir = normalize(-dirLight.direction);
	vec3 halfway = normalize(lightDir + fragToView);
	vec3 radiance = dirLight.lightColour;

	// Cook-Torrance Specular BRDF calculations
	float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
	vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
	float geometry = GeometrySmith(normal, fragToView, lightDir, roughness);

	// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
	vec3 specularRatio = fresnel;
	vec3 diffuseRatio = vec3(1.0) - specularRatio;
	diffuseRatio *= 1.0 - metallic;

	vec3 numerator = fresnel * normalDistribution * geometry;
	float denominator = 4 * max(dot(fragToView, normal), 0.0) * max(dot(lightDir, normal), 0.0) + 0.001;  // Prevents any division by zero
	vec3 specular = numerator / denominator;

	// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
	vec3 diffuse = diffuseRatio * albedo / PI;

	// Add the light's radiance to the irradiance sum
	return (diffuse + specular) * radiance * max(dot(normal, lightDir), 0.0);
}


vec3 CalculatePointLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 pointLightIrradiance = vec3(0.0);

	for (int i = 0; i < numPointLights; ++i) {
		vec3 fragToLight = normalize(pointLights[i].position - FragPos);
		vec3 halfway = normalize(fragToView + fragToLight);
		float fragToLightDistance = length(pointLights[i].position - FragPos);
		float attenuation = 1.0 / (fragToLightDistance * fragToLightDistance);
		vec3 radiance = pointLights[i].lightColour * attenuation;

		// Cook-Torrance Specular BRDF calculations
		float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
		vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
		float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

		// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
		vec3 specularRatio = fresnel;
		vec3 diffuseRatio = vec3(1.0) - specularRatio;
		diffuseRatio *= 1.0 - metallic;

		// Finally calculate the specular part of the Cook-Torrance BRDF
		vec3 numerator = specularRatio * normalDistribution * geometry;
		float denominator = 4 * max(dot(fragToView, normal), 0.0) * max(dot(fragToLight, normal), 0.0) + 0.001; // Prevents any division by zero
		vec3 specular = numerator / denominator;

		// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
		vec3 diffuse = diffuseRatio * albedo / PI;

		// Add the light's radiance to the irradiance sum
		pointLightIrradiance += (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
	}

	return pointLightIrradiance;
}


vec3 CalculateSpotLightRadiance(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 fragToView, vec3 baseReflectivity) {
	vec3 fragToLight = normalize(spotLight.position - FragPos);
	vec3 halfway = normalize(fragToView + fragToLight);
	float fragToLightDistance = length(spotLight.position - FragPos);

	// Check if it is in the spotlight's circle
	float theta = dot(normalize(spotLight.direction), -fragToLight);
	float difference = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - spotLight.outerCutOff) / difference, 0.0, 1.0);
	float attenuation = intensity * (1.0 / (fragToLightDistance * fragToLightDistance));
	vec3 radiance = spotLight.lightColour * attenuation;

	// Cook-Torrance Specular BRDF calculations
	float normalDistribution = NormalDistributionGGX(normal, halfway, roughness);
	vec3 fresnel = FresnelSchlick(max(dot(halfway, fragToView), 0.0), baseReflectivity);
	float geometry = GeometrySmith(normal, fragToView, fragToLight, roughness);

	// Calculate reflected and refracted light respectively, and since metals absorb all refracted light, we nullify the diffuse lighting based on the metallic parameter
	vec3 specularRatio = fresnel;
	vec3 diffuseRatio = vec3(1.0) - specularRatio;
	diffuseRatio *= 1.0 - metallic;

	// Finally calculate the specular part of the Cook-Torrance BRDF
	vec3 numerator = specularRatio * normalDistribution * geometry;
	float denominator = 4 * max(dot(fragToView, normal), 0.0) * max(dot(fragToLight, normal), 0.0) + 0.001; // Prevents any division by zero
	vec3 specular = numerator / denominator;

	// Also calculate the diffuse, a lambertian calculation will be added onto the final radiance calculation
	vec3 diffuse = diffuseRatio * albedo / PI;

	// Add the light's radiance to the irradiance sum
	return (diffuse + specular) * radiance * max(dot(normal, fragToLight), 0.0);
}


// Approximates the amount of microfacets that are properly aligned with the halfway vector, thus determines the strength and area for specular light
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float normDotHalf = dot(normal, halfway);
	float normDotHalf2 = normDotHalf * normDotHalf;

	float numerator = a2;
	float denominator = normDotHalf2 * (a2 - 1) + 1;
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


float CalculateShadow(vec3 normal, vec3 fragToDirLight) {
	vec3 ndcCoords = FragPosLightClipSpace.xyz / FragPosLightClipSpace.w;
	vec3 depthmapCoords = ndcCoords * 0.5 + 0.5;

	float shadow = 0.0;
	float currentDepth = depthmapCoords.z;

	// Add shadow bias to avoid shadow acne, and more shadow bias is needed depending on the angle between the normal and light direction
	// However too much bias can cause peter panning
	float shadowBias = max(0.01, 0.1 * (1.0 - dot(normal, fragToDirLight)));

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