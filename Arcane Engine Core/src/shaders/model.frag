#version 430 core

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D texture_diffuse;
	sampler2D texture_specular;
	sampler2D texture_normal;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define MAX_POINT_LIGHTS 5

in mat3 TBN;
in vec3 FragPos;
in vec4 FragPosLightClipSpace;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform float time;

uniform sampler2D shadowmap;
uniform int numPointLights;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragToCam);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCam);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 fragToCam);
float CalculateShadow(vec3 normal, vec3 fragToDirLight);

void main() {
	// Check if the fragment is too transparent, and if so just discard it
	float textureAlpha = texture(material.texture_diffuse, TexCoords).w;

	// Normal mapping code. Opted out of tangent space normal mapping since I would have to convert all of my lights to tangent space
	vec3 norm = texture(material.texture_normal, TexCoords).rgb;
	norm = normalize(norm * 2.0f - 1.0f);
	norm = normalize(TBN * norm);
	
	vec3 fragToCam = normalize(viewPos - FragPos);
	
	vec3 result = CalcDirLight(dirLight, norm, fragToCam);
	for(uint i = 0; i < numPointLights; ++i) {
		result += CalcPointLight(pointLights[i], norm, FragPos, fragToCam);
	}
	result += CalcSpotLight(spotLight, norm, FragPos, fragToCam);
	
	// Result
	color = vec4(result, textureAlpha); // Use the diffuse texture for transparency
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragToCam) {
	vec3 fragToLight = normalize(-light.direction);

	float diff = max(dot(fragToLight, normal), 0.0);

	vec3 halfwayDir = normalize(fragToCam + fragToLight);
	float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);

	vec3 ambient = light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.texture_specular, TexCoords).rgb;
	//vec3 emission = texture(material.emission, TexCoords).rgb * clamp((sin(time) * 2) - 1, 0, 1);
	
	return (ambient + (diffuse + specular) * (1.0 - CalculateShadow(normal, fragToLight)));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCam) {
	vec3 lightDirection = normalize(light.position - fragPos);

	float diff = max(dot(lightDirection, normal), 0.0);

	vec3 halfwayDir = normalize(lightDirection + fragToCam);
	float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);

	// Attenuation calculation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.texture_specular, TexCoords).rgb;
	//vec3 emission = texture(material.emission, TexCoords).rgb * clamp((sin(time) * 2) - 1, 0, 1);

	// Apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 fragToCam) {
	vec3 fragToLight = normalize(light.position - fragPos);

	float diff = max(dot(fragToLight, normal), 0.0);

	vec3 halfwayDir = normalize(fragToLight + fragToCam);
	float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);

	// Attenuation calculation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	// Check if it is in the spotlight's circle
	float theta = dot(normalize(light.direction), -fragToLight);
	float difference = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / difference, 0.0, 1.0);

	vec3 ambient = light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.texture_specular, TexCoords).rgb;
	//vec3 emission = texture(material.emission, TexCoords).rgb * clamp((sin(time) * 2) - 1, 0, 1);

	// Apply attenuation
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
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