#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * (vec4(position, 1.0f));
	FragPos = vec3(model * vec4(position, 1.0f));
	TexCoords = texCoords;

	// Use the normal matrix to maintain the orthogonal property of a normal when it is scaled non-uniformly
	// Inverse is a very costly function, maybe do it on the cpu then send it to the gpu as a uniform
	// Also this saves how many times this costly calculation is done, since right now it is happening every vertex, instead of once per triangle
	Normal = mat3(transpose(inverse(model))) * normal;
}