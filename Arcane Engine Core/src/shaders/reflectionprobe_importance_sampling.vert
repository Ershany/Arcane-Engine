#version 430 core

/*
	This IBL - Reflection Probe Importance Sampling approach is based on the approach Epic made in their paper "Real Shading in Unreal Engine 4"
*/

layout (location = 0) in vec3 position;

out vec3 SampleDirection;

uniform mat4 view;
uniform mat4 projection;

void main() {
	SampleDirection = position;

	// Cut off the translation part of the view matrix (so the cube will not be translated because we want the cube to surround the probe's origin)
	gl_Position = projection * mat4(mat3(view)) * vec4(position, 1.0f); 
}
