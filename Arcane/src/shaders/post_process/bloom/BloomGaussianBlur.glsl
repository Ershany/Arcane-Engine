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

uniform sampler2D bloom_texture;

uniform bool isVerticalBlur;
uniform vec2 read_offset;

const float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
	vec3 blurResult = texture(bloom_texture, TexCoords).rgb * weights[0];
	if (isVerticalBlur) {
		for (int i = 1; i < 5; ++i) {
			blurResult += texture2D(bloom_texture, TexCoords + vec2(0.0, i * read_offset.y)).rgb * weights[i];
			blurResult += texture2D(bloom_texture, TexCoords + vec2(0.0, i * -read_offset.y)).rgb * weights[i];
		}
	}
	else {
		for (int i = 1; i < 5; ++i) {
			blurResult += texture2D(bloom_texture, TexCoords + vec2(i * read_offset.y, 0.0)).rgb * weights[i];
			blurResult += texture2D(bloom_texture, TexCoords + vec2(i * -read_offset.y, 0.0)).rgb * weights[i];
		}
	}
	FragColour = vec4(blurResult, 1.0);
}
