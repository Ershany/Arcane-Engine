#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoords;

void main() {
	gl_Position = vec4(position, 1.0);
	TexCoords = texCoord;
}




#shader-type fragment
#version 430 core

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;

uniform float bloomStrength;

void main() {
	vec3 hdrScene = texture2D(sceneTexture, TexCoords).rgb;
	vec3 hdrBloom = texture2D(bloomTexture, TexCoords).rgb;
	FragColour = vec4(hdrScene + (hdrBloom * bloomStrength), 1.0); // Additive blend
}
