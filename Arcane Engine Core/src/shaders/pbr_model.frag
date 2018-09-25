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

uniform sampler2D shadowmap;
uniform int numPointLights;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;

// Cook-Torrance BRDF function prototypes
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
float GeometrySchlickGGX(vec3 cosTheta, float roughness);
vec3 FresnelSchlick(vec3 cosTheta, vec3 baseReflectivity);

// Other function prototypes
float CalculateShadow(vec3 normal, vec3 fragToDirLight);

void main() {
	// Normal mapping code. Opted out of tangent space normal mapping since I would have to convert all of my lights to tangent space
	vec3 normal = texture(material.texture_normal, TexCoords).rgb;
	normal = normalize(norm * 2.0f - 1.0f);
	normal = normalize(TBN * norm);
	
	vec3 fragToView = normalize(viewPos - FragPos);

	vec3 irradiance = vec3(0.0);
	

	color = vec4(0.0, 1.0, 1.0, 1.0);
	/*
	vec3 result = CalcDirLight(dirLight, norm, fragToCam);
	for(uint i = 0; i < numPointLights; ++i) {
		result += CalcPointLight(pointLights[i], norm, FragPos, fragToCam);
	}
	result += CalcSpotLight(spotLight, norm, FragPos, fragToCam);
	
	// Result
	color = vec4(result, textureAlpha); // Use the diffuse texture for transparency
	*/
}

// Approximates the amount of microfacets that are properly aligned with the halfway vector, thus determines the strength and area for specular light
float NormalDistributionGGX(vec3 normal, vec3 halfway, float roughness) {
	float roughness2 = roughness * roughness;
	float normDotHalf = dot(normal, halfway);
	float normDotHalf2 = normDotHalf * normDotHalf;

	float numerator = roughness2;
	float denominator = normDotHalf2 * (roughness2 - 1) + 1;
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

// Approximates the geometry obstruction and geometry shadowing respectively, on the microfacet level
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
	return GeometrySchlickGGX(max(dot(normal, viewDir), 1.0), roughness) * GeometrySchlickGGX(max(dot(normal, lightDir), 1.0), roughness);
}
float GeometrySchlickGGX(vec3 cosTheta, float roughness) {
	float numerator = cosTheta;
	float denominator = cosTheta * (1.0 - roughness) + roughness;
	return numerator / denominator;
}

// Calculates the amount of specular (reflected) light. Since diffuse(refraction) and specular(reflection) are mutually exclusive, 
// we can also use this to determine the amount of diffuse light
vec3 FresnelSchlick(vec3 cosTheta, vec3 baseReflectivity) {
	return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - cosTheta, 5);
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