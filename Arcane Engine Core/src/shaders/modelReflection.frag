#version 430 core

in vec3 FragPos;
in vec3 Normal;

out vec4 colour;

uniform vec3 cameraPos;
uniform samplerCube environmentMap;

void main() {
	vec3 fragToCam = normalize(FragPos - cameraPos);
	vec3 reflectedDir = reflect(fragToCam, normalize(Normal));

	colour = texture(environmentMap, reflectedDir);
}