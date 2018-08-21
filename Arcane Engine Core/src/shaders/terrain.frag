#version 430 core

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D texture_diffuse1; // background texture
	sampler2D texture_diffuse2; // r texture
	sampler2D texture_diffuse3; // g texture
	sampler2D texture_diffuse4; // b texture
	sampler2D texture_diffuse5; // blend map
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

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightClipSpace;

out vec4 color;

uniform sampler2D shadowmap;
uniform int numPointLights;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;

// Functions
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragToCam);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCam);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos);
float CalculateShadow(vec3 normal, vec3 fragToDirLight);

void main() {
	vec3 norm = normalize(Normal);
	vec3 fragToCam = normalize(viewPos - FragPos);

	vec4 blendMapColour = texture(material.texture_diffuse5, TexCoords);
	
	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	
	vec2 tiledCoords = TexCoords * 128;
	vec3 backgroundTextureColour = texture(material.texture_diffuse1, tiledCoords).rgb * backTextureAmount;
	vec3 rTextureColour = texture(material.texture_diffuse2, tiledCoords).rgb * blendMapColour.r;
	vec3 gTextureColour = texture(material.texture_diffuse3, tiledCoords).rgb * blendMapColour.g;
	vec3 bTextureColour = texture(material.texture_diffuse4, tiledCoords).rgb * blendMapColour.b;
	
	vec3 terrainColour = (backgroundTextureColour + rTextureColour + gTextureColour + bTextureColour) * 
						 (CalcDirLight(dirLight, norm, fragToCam) + CalcSpotLight(spotLight, norm, FragPos) + CalcPointLight(pointLights[0], norm, FragPos, fragToCam));
	
	
	// Result
	color = vec4(terrainColour, 1.0);
	//color = vec4(Normal, 1.0);
	//color = vec4(vec3(gl_FragCoord.z), 1.0); //depth buffer display
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragToCam) {
	vec3 fragToLight = normalize(-light.direction);

	float diff = max(dot(fragToLight, normal), 0.0);

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
	
	return ambient + (diffuse * (1.0 - CalculateShadow(normal, fragToLight)));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCam) {
	vec3 fragToLight = normalize(light.position - fragPos);

	float diff = max(dot(fragToLight, normal), 0.0);

	// Attenuation calculation
	float distanceFromLight = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distanceFromLight + light.quadratic * distanceFromLight * distanceFromLight);

	vec3 ambient = light.ambient * attenuation;
	vec3 diffuse = light.diffuse * diff * attenuation;

	return ambient + diffuse;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos) {
	vec3 fragToLight = normalize(light.position - fragPos);

	// Attenuation calculation
	float distanceFromLight = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distanceFromLight + light.quadratic * distanceFromLight * distanceFromLight);

	// check if its in the spotlight's circle
	float theta = dot(-fragToLight, normalize(light.direction));
	float difference = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / difference, 0.0, 1.0);

	vec3 ambient = light.ambient * attenuation;
	vec3 diffuse = light.diffuse * intensity * attenuation;

	return ambient + diffuse;
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
