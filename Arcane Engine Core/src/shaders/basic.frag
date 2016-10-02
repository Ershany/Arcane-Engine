#version 450 core

out vec4 color;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main() {
	color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.8) * vec4(ourColor, 1.0f);
}