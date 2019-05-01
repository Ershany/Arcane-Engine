#version 430 core

out vec4 FragColour;

in vec3 SampleDirection;

uniform samplerCube skyboxCubemap;

void main() {
	// Sample direction is the hemisphere's orientation for convolution
	vec3 normal = normalize(SampleDirection);

	// Do the convolution (ie search the hemisphere and sample)
	vec3 hemisphereIrradiance = vec3(0.0, 0.0, 0.0);


	FragColour = vec4(hemisphereIrradiance, 1.0);
}