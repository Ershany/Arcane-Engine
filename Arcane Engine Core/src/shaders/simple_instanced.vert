#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 instancePos;

uniform mat4 view;
uniform mat4 projection;
uniform float yOffset;

void main() {
	gl_Position = projection * view * vec4(position + instancePos + vec3(0.0, yOffset, 0.0), 1.0f);
}