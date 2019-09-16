/*
	Deferred SSAO Generation. It tiles a 4x4 texture accross the screen, in order to elimate banding but then it will introduce high frequency noise that can be blurred
*/

#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = texCoords;
	gl_Position = vec4(position, 1.0);
}




#shader-type fragment
#version 430 core

in vec2 TexCoords;

out float FragColour;

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D texNoise;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 noiseScale;

uniform float ssaoStrength;
uniform float sampleRadius;
uniform float sampleRadius2;
uniform int numKernelSamples;
uniform vec3 samples[64];

uniform mat4 view;
uniform mat4 projection;
uniform mat4 viewInverse;
uniform mat4 projectionInverse;

// Other function prototypes
vec3 WorldPosFromDepth(vec2 textureCoordinates);

void main() {
	vec3 fragPos = WorldPosFromDepth(TexCoords);
	vec3 normal = texture(normalTexture, TexCoords).xyz;
	vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

	// Make a TBN matrix to go from tangent -> world space (so we can put our hemipshere tangent sample points into world space)
	// Since our normal is already in world space, this TBN matrix will take our tangent space vector and put it into world space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// Calculate the total amount of occlusion for this fragment
	float occlusion = 0.0f;
	for (int i = 0; i < numKernelSamples; ++i) {
		vec3 sampleWorld = (TBN * samples[i]) * sampleRadius;
		sampleWorld = fragPos + sampleWorld;

		// Take our sample position in world space and convert it to screen coordinates
		vec4 sampleScreenSpace = vec4(sampleWorld, 1.0);
		sampleScreenSpace = projection * view * sampleScreenSpace; // World -> View -> Clip Space
		sampleScreenSpace.xyz /= sampleScreenSpace.w; // Perspective division (Clip Space -> NDC)
		sampleScreenSpace.xyz = (sampleScreenSpace.xyz * 0.5) + 0.5; // [-1, 1] -> [0, 1]

		// Check if our current samples depth is behind the screen space geometry's depth, if so then we know it is occluded in screenspace
		float sceneDepth = texture(depthTexture, sampleScreenSpace.xy).r;

		// Peform a range check on the current fragment we are calculating the occlusion factor for, and the occlusion position
		vec3 occlusionPos = WorldPosFromDepth(sampleScreenSpace.xy);
		vec3 fragToOcclusionPos = fragPos - occlusionPos;
		if (dot(fragToOcclusionPos, fragToOcclusionPos) <= sampleRadius2) {
			occlusion += ((sampleScreenSpace.z > sceneDepth) ? 1.0 : 0.0);
		}
	}
	// Finally we need to normalize our occlusion factor
	occlusion = 1.0 - (occlusion / numKernelSamples);

	FragColour = pow(occlusion, ssaoStrength);
}

vec3 WorldPosFromDepth(vec2 textureCoordinates) {
	float z = 2.0 * texture(depthTexture, textureCoordinates).r - 1.0; // [-1, 1]
	vec4 clipSpacePos = vec4(textureCoordinates * 2.0 - 1.0 , z, 1.0);
	vec4 viewSpacePos = projectionInverse * clipSpacePos;

	viewSpacePos /= viewSpacePos.w; // Perspective division

	vec4 worldSpacePos = viewInverse * viewSpacePos;

	return worldSpacePos.xyz;
}
