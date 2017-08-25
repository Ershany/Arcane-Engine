#version 430 core

layout (location = 0) in vec3 position;

out vec3 SampleDirection;

uniform mat4 view;
uniform mat4 projection;

void main() {
	// Cut off the translation part of the view matrix (so the skybox will not move when the camera is moved), but keep the other parts so it can rotate with the camera
	gl_Position = projection * mat4(mat3(view)) * vec4(position, 1.0f);
	SampleDirection = position; // A skymap can be sampled by its vertex positions (since it is centered around the origin)
}