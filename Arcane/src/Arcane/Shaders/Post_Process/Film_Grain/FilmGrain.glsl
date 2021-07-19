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

uniform sampler2D input_texture;

uniform float intensity;
uniform float time;

void main() {
	vec3 colour = texture2D(input_texture, TexCoords).rgb;

	float x = (TexCoords.x + 4) * (TexCoords.y + 4) * ((time + 1) * 10.0);
	vec4 grain = vec4(mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01) - 0.005) * intensity;

	FragColour = vec4(colour + grain.xyz, 1.0);
}
