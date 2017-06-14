// simple point light source (infinite range)

#version 450 core

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform float time;

void main() {
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos); // negate because calculations expect it to be pointing from the fragment to the light source

	// Ambient
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	// Diffuse
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	// Specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // reflect assumes the first vector points from the light source towards the fragments position
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	// Emission
	vec3 emission = vec3(texture(material.emission, TexCoords)) + abs(sin(time));

	// Point light attenuation calculations
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); // Realistic point light fading
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// Result
	color = vec4(ambient + diffuse + specular, 1.0f);
}