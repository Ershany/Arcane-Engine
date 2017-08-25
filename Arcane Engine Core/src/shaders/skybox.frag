#version 430 core

out vec4 FragColour;

in vec3 SampleDirection;

uniform samplerCube skyboxCubemap;

void main() {
	FragColour = texture(skyboxCubemap, SampleDirection);
}