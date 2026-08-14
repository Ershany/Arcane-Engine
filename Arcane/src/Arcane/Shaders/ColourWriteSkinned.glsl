#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
const int MAX_BONES_PER_VERTEX = 4;
uniform mat4 bonesMatrices[MAX_BONES];

void main() {
	mat4 boneTransform = bonesMatrices[boneIds[0]] * weights[0] +
						 bonesMatrices[boneIds[1]] * weights[1] +
						 bonesMatrices[boneIds[2]] * weights[2] +
						 bonesMatrices[boneIds[3]] * weights[3];

	gl_Position = projection * view * model * boneTransform * vec4(position, 1.0);
}




#shader-type fragment
#version 430 core 

out vec4 FragColour;

uniform vec3 colour;

void main() {
	FragColour = vec4(colour, 1.0f);
}
