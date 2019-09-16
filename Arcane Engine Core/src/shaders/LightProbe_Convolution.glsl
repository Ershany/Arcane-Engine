#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;

out vec3 SampleDirection;

uniform mat4 view;
uniform mat4 projection;

void main() {
	SampleDirection = position;

	// Cut off the translation part of the view matrix (so the cube will not be translated because we want the cube to surround the probe's origin)
	gl_Position = projection * mat4(mat3(view)) * vec4(position, 1.0f); 
}




#shader-type fragment
#version 430 core

out vec4 FragColour;

in vec3 SampleDirection;

uniform samplerCube sceneCaptureCubemap;

const float PI = 3.14159265359;

void main() {
	// Sample direction is the hemisphere's orientation for convolution
	vec3 normal = normalize(SampleDirection);

	// We need the axis to convert our vector from tangent to world space (since the cubemap we took is in world space)
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);

	// Search the hemisphere and sample for the convolution
	vec3 hemisphereIrradiance = vec3(0.0, 0.0, 0.0);
	float numSamples = 0;

	float delta = 0.05; // 0.05 results in about 126 * 32 = 4032 samples
	for (float phi = 0.0; phi < PI * 2.0; phi += delta) { // Ring of the hemisphere
		for (float theta = 0.0; theta < PI * 0.5; theta += delta) { // For the increasing rings on the hemisphere
			// Tangent Space
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));

			// Tangent to World Space
			vec3 worldSample = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			// Scaled by sin(theta) to account for smaller sample areas in the higher hemipshere rings, and scaled by cos(theta) due to light being weaker at larger angles
			hemisphereIrradiance += texture(sceneCaptureCubemap, worldSample).rgb * cos(theta) * sin(theta);
			++numSamples;
		}
	}

	vec3 irradiance = PI * hemisphereIrradiance * (1.0 / float(numSamples));
	FragColour = vec4(irradiance, 1.0);
}
