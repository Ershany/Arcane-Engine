#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	ourColor = color;
	TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);
}