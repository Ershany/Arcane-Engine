#version 450 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {
	// Ambient
	vec3 ambient = material.ambient * light.ambient;

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos); // points from the fragments position to the light source
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // reflect assumes the first vector points from the light source towards the fragments position
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	// Result
	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0f);
}