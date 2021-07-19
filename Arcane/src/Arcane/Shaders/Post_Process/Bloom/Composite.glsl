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

uniform sampler2D scene_texture;
uniform sampler2D bloom_texture;

uniform float strength;

void main() {
	vec3 hdrScene = texture2D(scene_texture, TexCoords).rgb;
	vec3 hdrBloom = texture2D(bloom_texture, TexCoords).rgb;
	
	FragColour = vec4(hdrScene + hdrBloom * strength, 1.0);
}
