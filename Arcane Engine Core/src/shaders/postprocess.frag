#version 430 core

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D colour_texture;
uniform float gamma_inverse;
uniform float exposure;
uniform vec2 read_offset;

void main() {
	vec3 hdrColour = texture(colour_texture, TexCoords).rgb;
	
	// Apply a simple exposure tonemap (HDR -> SDR) (dark scenes should have higher exposures, while bright scenes should have lower exposures)
	vec3 tonemappedColour = vec3(1.0) - exp(exposure * -hdrColour);

	// Apply gamma correction (Linear -> Non-linear)
	FragColour = vec4(pow(tonemappedColour, vec3(gamma_inverse)), 1.0);
}
