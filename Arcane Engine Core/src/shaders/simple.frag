#version 430 core

uniform vec3 colour;
uniform float alpha;

out vec4 FragColour;

void main() {
	FragColour = vec4(colour, alpha);
}