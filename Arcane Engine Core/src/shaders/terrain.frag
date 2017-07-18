#version 450 core

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

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;


uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;

// Functions
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragToCamera);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCamera);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos);

void main() {
	vec3 norm = normalize(Normal);
	vec3 fragToCamera = normalize(viewPos - FragPos);

	vec4 blendMapColour = texture(material.texture_diffuse5, TexCoords);
	
	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	
	vec2 tiledCoords = TexCoords * 128;
	vec3 backgroundTextureColour = texture(material.texture_diffuse1, tiledCoords).rgb * backTextureAmount;
	vec3 rTextureColour = texture(material.texture_diffuse2, tiledCoords).rgb * blendMapColour.r;
	vec3 gTextureColour = texture(material.texture_diffuse3, tiledCoords).rgb * blendMapColour.g;
	vec3 bTextureColour = texture(material.texture_diffuse4, tiledCoords).rgb * blendMapColour.b;
	
	vec3 terrainColour = (backgroundTextureColour + rTextureColour + gTextureColour + bTextureColour) * 
						 (CalcDirLight(dirLight, norm, fragToCamera) + CalcSpotLight(spotLight, norm, FragPos) + CalcPointLight(pointLight, norm, FragPos, fragToCamera));
	
	
	// Result
	color = vec4(terrainColour, 1.0);
	//color = vec4(Normal, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragToCamera) {
	vec3 lightDirection = normalize(light.direction);

	float diff = max(dot(-lightDirection, normal), 0.0);

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;

	return ambient + diffuse;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 fragToCamera) {
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