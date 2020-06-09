#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec4 clipSpace;
out vec2 planeTexCoords;
out vec3 fragToView;

uniform vec3 viewPos;
uniform float waveTiling;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

void main() {
	vec4 worldPos = model * vec4(position, 1.0);
	clipSpace = projection * view *worldPos;
	planeTexCoords = texCoords * waveTiling;
	fragToView = viewPos - vec3(worldPos);

	gl_Position = clipSpace;
}




#shader-type fragment
#version 430 core

in vec4 clipSpace;
in vec2 planeTexCoords;
in vec3 normal;
in vec3 fragToView;

out vec4 FragColour;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvWaveTexture;
uniform sampler2D normalMap;

uniform bool clearWater;
uniform vec3 waterAlbedo;
uniform float albedoPower;
uniform float waveMoveFactor;
const float waveStrength = 0.02f;

void main() {
	vec2 ndc = clipSpace.xy / clipSpace.w;
	vec2 textureCoords = ndc * 0.5 + 0.5;

	vec2 reflectCoords = vec2(textureCoords.x, 1.0 - textureCoords.y);
	vec2 refractCoords = vec2(textureCoords.x, textureCoords.y);

	vec2 totalDistortion;
	if (clearWater) {
		totalDistortion = vec2(0.0, 0.0);
	}
	else {
		vec2 distortion = (texture(dudvWaveTexture, vec2(planeTexCoords.x + waveMoveFactor, planeTexCoords.y)).rg * 2.0 - 1.0) * 0.1; // Unpack the dudv map
		distortion = planeTexCoords + vec2(distortion.x, distortion.y + waveMoveFactor);
		totalDistortion = (texture(dudvWaveTexture, distortion).rg * 2.0 - 1.0) * waveStrength; // Unpack the dudv map and multiply by strength
	}
	

	reflectCoords += totalDistortion;
	reflectCoords = clamp(reflectCoords, 0.0, 1.0);

	refractCoords += totalDistortion;
	refractCoords = clamp(refractCoords, 0.0, 1.0);

	vec4 reflectedColour = texture(reflectionTexture, reflectCoords);
	vec4 refractedColour = texture(refractionTexture, refractCoords);

	float fresnel = dot(normalize(fragToView), vec3(0.0, 1.0, 0.0));
	//vec3 normal = texture(normalMap, distortion).rgb;

	FragColour = mix(reflectedColour, refractedColour, fresnel);
	FragColour = mix(FragColour, vec4(waterAlbedo, 1.0), albedoPower);
}
