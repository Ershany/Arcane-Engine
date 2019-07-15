#version 430 core

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D colour_texture;
uniform float gamma_inverse;
uniform vec2 read_offset;

void main() {
	vec3 hdrColour = texture(colour_texture, TexCoords).rgb;
	
	// Apply a simple Reinhard tonemapper (HDR -> SDR)
	vec3 tonemappedColour = hdrColour / (hdrColour + vec3(1.0));

	// Apply gamma correction (Linear -> Non-linear)
	FragColour = vec4(pow(tonemappedColour, vec3(gamma_inverse)), 1.0);
}
