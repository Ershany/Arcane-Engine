#version 430 core

layout (location = 0) in vec3 position;

uniform mat4 lightSpaceViewProjectionMatrix;
uniform mat4 model;

void main() {
	gl_Position = lightSpaceViewProjectionMatrix * model * vec4(position, 1.0f);
}