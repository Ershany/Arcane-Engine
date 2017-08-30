#version 430 core

in vec3 FragPos;
in vec3 Normal;

out vec4 colour;

uniform vec3 cameraPos;
uniform samplerCube environmentMap;

void main() {
	float refractionIndex = 1.00f/1.52f; // Assume light is travelling through air and the model is glass
	vec3 viewDir = normalize(FragPos - cameraPos);
	vec3 refractedDir = refract(viewDir, normalize(Normal), refractionIndex);

	colour = texture(environmentMap, refractedDir);
}