#version 430 core

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D texture_diffuse;
	sampler2D texture_specular;
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

out vec4 color;


uniform int numPointLights;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform float time;

uniform Material material;
uniform vec3 viewPos;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragToCam);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCam);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 fragToCam);

void main() {
	// Check if the fragment is too transparent, and if so just discard it
	float textureAlpha = texture(material.texture_diffuse, TexCoords).w;

	vec3 norm = normalize(Normal);
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
	vec3 lightDirection = normalize(-light.direction);

	float diff = max(dot(lightDirection, normal), 0.0);

	vec3 halfwayDir = normalize(fragToCam + lightDirection);
	float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);

	vec3 ambient = light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.texture_specular, TexCoords).rgb;
	//vec3 emission = texture(material.emission, TexCoords).rgb * clamp((sin(time) * 2) - 1, 0, 1);
	
	return (ambient + diffuse + specular);
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