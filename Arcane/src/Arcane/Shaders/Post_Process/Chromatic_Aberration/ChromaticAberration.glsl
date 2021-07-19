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
uniform vec2 texel_size;

void main() {
	vec2 diffFromCenter = TexCoords - vec2(0.5, 0.5);
	vec2 offset = diffFromCenter * texel_size * intensity;

	// Emulated how Unity handles their "fast" implementation
	float r = texture2D(input_texture, TexCoords - (1 * offset)).r;
	float g = texture2D(input_texture, TexCoords - (2 * offset)).g;
	float b = texture2D(input_texture, TexCoords - (3 * offset)).b;

	FragColour = vec4(r, g, b, 1.0);
}
