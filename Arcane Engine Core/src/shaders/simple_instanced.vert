#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 instancePos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * vec4(position + instancePos, 1.0f);
}