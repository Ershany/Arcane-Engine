#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;

out vec4 worldFragPos;

uniform mat4 lightSpaceViewProjectionMatrix;
uniform mat4 model;

void main() {
	worldFragPos = model * vec4(position, 1.0f);
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
