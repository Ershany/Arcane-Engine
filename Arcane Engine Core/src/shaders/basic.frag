#version 450 core

out vec4 color;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPosition;
uniform vec3 viewPos;

void main() {
	// Ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColour;

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPosition - FragPos); // points from the fragments position to the light source
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColour;

	// Specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // reflect assumes the first vector points from the light source towards the fragments position
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64); // 32 is the shininess value
	vec3 specular = specularStrength * spec * lightColour;

	// Result
	vec3 result = (ambient + diffuse + specular) * objectColour;
	color = vec4(result, 1.0f);
}