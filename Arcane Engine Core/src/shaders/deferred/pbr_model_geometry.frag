#version 430 core

layout (location = 0) out vec4 gb_Albedo;
layout (location = 1) out vec3 gb_Normal;
layout (location = 2) out vec4 gb_MaterialInfo;

// Does AMD support sampler2D in a struct?
struct Material {
	sampler2D texture_albedo;
	sampler2D texture_normal;
	sampler2D texture_metallic;
	sampler2D texture_roughness;
	sampler2D texture_ao;
	sampler2D texture_displacement;
};

in mat3 TBN;
in vec2 TexCoords;
in vec3 FragPosTangentSpace;
in vec3 ViewPosTangentSpace;

uniform bool hasDisplacement;
uniform vec2 minMaxDisplacementSteps;
uniform float parallaxStrength;
uniform Material material;

// Functions
vec3 UnpackNormal(vec3 textureNormal);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDirTangentSpace);

void main() {
	// Parallax mapping
	vec2 textureCoordinates = TexCoords;
	if (hasDisplacement) {
		vec3 viewDirTangentSpace = normalize(ViewPosTangentSpace - FragPosTangentSpace);
		textureCoordinates = ParallaxMapping(TexCoords, viewDirTangentSpace);
	}

	// Sample textures
	vec4 albedo = texture(material.texture_albedo, textureCoordinates);
	vec3 normal = texture(material.texture_normal, textureCoordinates).rgb;
	float metallic = texture(material.texture_metallic, textureCoordinates).r;
	float roughness = max(texture(material.texture_roughness, textureCoordinates).r, 0.04);
	float ao = texture(material.texture_ao, textureCoordinates).r;

	// Normal mapping code. Opted out of tangent space normal mapping since I would have to convert all of my lights to tangent space
	normal = normalize(TBN * UnpackNormal(normal));

	gb_Albedo = albedo;
	gb_Normal = normal;
	gb_MaterialInfo = vec4(metallic, roughness, ao, 1.0);
}

// Unpacks the normal from the texture and returns the normal in tangent space
vec3 UnpackNormal(vec3 textureNormal) {
	return normalize(textureNormal * 2.0 - 1.0);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDirTangentSpace) {
	// Figure out the LoD we should sample from while raymarching the heightfield in tangent space. Required to fix an artifacting issue
	vec2 lodInfo = textureQueryLod(material.texture_displacement, texCoords);
	float lodToSample = lodInfo.x;
	float expectedLod = lodInfo.y; // Even if mip mapping isn't enabled this will still give us a mip level

	const float minSteps = minMaxDisplacementSteps.x;
	const float maxSteps = minMaxDisplacementSteps.y;
	float numSteps = mix(maxSteps, minSteps, clamp(expectedLod * 0.4, 0, 1)); // More steps are required at lower mip levels since the camera is closer to the surface

	float layerDepth = 1.0 / numSteps;
	float currentLayerDepth = 0.0;

	// Calculate the direction and the amount we should raymarch each iteration
	vec2 p = viewDirTangentSpace.xy * parallaxStrength;
	vec2 deltaTexCoords = p / numSteps;

	// Get the initial values
	vec2 currentTexCoords = texCoords;
	float currentSampledDepth = textureLod(material.texture_displacement, currentTexCoords, lodToSample).r;

	// Keep ray marching along vector p by the texture coordinate delta, until the raymarching depth catches up to the sampled depth (ie the -view vector intersects the surface)
	while (currentLayerDepth < currentSampledDepth) {
		currentTexCoords -= deltaTexCoords;
		currentSampledDepth = textureLod(material.texture_displacement, currentTexCoords, lodToSample).r;
		currentLayerDepth += layerDepth;
	}

	// Now we need to get the previous step and the current step, and interpolate between the two texture coordinates
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	float afterDepth = currentSampledDepth - currentLayerDepth;
	float beforeDepth = textureLod(material.texture_displacement, prevTexCoords, lodToSample).r - currentLayerDepth + layerDepth;
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = mix(currentTexCoords, prevTexCoords, weight);

	return finalTexCoords;
}
