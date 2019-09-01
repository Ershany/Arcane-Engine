#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out mat3 TBN;
out vec2 TexCoords;
out vec3 FragPos;
out vec4 FragPosLightClipSpace;
out vec3 FragPosTangentSpace;
out vec3 ViewPosTangentSpace;

uniform bool hasDisplacement;
uniform vec3 viewPos;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceViewProjectionMatrix;

void main() {
	// Use the normal matrix to maintain the orthogonal property of a vector when it is scaled non-uniformly
	vec3 T = normalize(normalMatrix * tangent);
	vec3 B = normalize(normalMatrix * bitangent);
	vec3 N = normalize(normalMatrix * normal);
	TBN = mat3(T, B, N);

	TexCoords = texCoords;
	FragPos = vec3(model * vec4(position, 1.0f));
	FragPosLightClipSpace = lightSpaceViewProjectionMatrix * vec4(FragPos, 1.0);
	if (hasDisplacement) {
		mat3 inverseTBN = transpose(TBN); // Calculate matrix to go from world -> tangent (orthogonal matrix's transpose = inverse)
		FragPosTangentSpace = inverseTBN * FragPos;
		ViewPosTangentSpace = inverseTBN * viewPos;
	}

	gl_Position = projection * view * vec4(FragPos, 1.0);
}
