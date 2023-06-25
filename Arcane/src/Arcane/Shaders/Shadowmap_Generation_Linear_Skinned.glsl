#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

out vec4 worldFragPos;

uniform mat4 lightSpaceViewProjectionMatrix;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONES_PER_VERTEX = 4;
uniform mat4 bonesMatrices[MAX_BONES];

void main() {
	mat4 boneTransform = bonesMatrices[boneIds[0]] * weights[0] +
						 bonesMatrices[boneIds[1]] * weights[1] +
						 bonesMatrices[boneIds[2]] * weights[2] +
						 bonesMatrices[boneIds[3]] * weights[3];

	worldFragPos = model * boneTransform * vec4(position, 1.0f);
	gl_Position = lightSpaceViewProjectionMatrix * worldFragPos;
}




#shader-type fragment
#version 430 core

in vec4 worldFragPos;

uniform vec3 lightPos;
uniform float lightFarPlane;

void main() {
	float lightDistance = length(worldFragPos.xyz - lightPos);
	lightDistance = lightDistance / lightFarPlane; // Map value to [0, 1]
	gl_FragDepth = lightDistance;
}
