#version 430 core

out vec4 FragColour;

in vec2 TexCoords;

uniform sampler2D screen_texture;
uniform float gamma_inverse;
uniform vec2 read_offset;
uniform bool blur_enabled;


void main() {
	// Sample the fragments around the current fragment for post processing using kernels (convulution matrices)
	// Note: Post processing may cause aliasing since it uses the blitted framebuffer (A non-multisampled buffer)
	vec2 readOffsets[9] = vec2[] (
		vec2(-read_offset.x, read_offset.y),
		vec2(0.0, read_offset.y),
		vec2(read_offset.x, read_offset.y),
		vec2(-read_offset.x, 0.0),
		vec2(0.0, 0.0),
		vec2(read_offset.x, 0.0),
		vec2(-read_offset.x, -read_offset.y),
		vec2(0.0, -read_offset.y),
		vec2(read_offset.x, -read_offset.y)
	);

	vec3 hdrColour = vec3(0.0);
	if (blur_enabled) {
		// Blur kernel
		float kernel[9] = float[] (
			1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
			2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
			1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
		);

		// Apply the kernel (post-processing effect)
		for(int i = 0; i < 9; ++i) {
			hdrColour += texture(screen_texture, TexCoords + readOffsets[i]).rgb * kernel[i];
		}
	}
	else {
		hdrColour = texture(screen_texture, TexCoords).rgb;
	}
	
	// Apply a simple Reinhard tonemapper (HDR -> SDR)
	vec3 tonemappedColour = hdrColour / (hdrColour + vec3(1.0));

	// Apply gamma correction
	FragColour = vec4(pow(tonemappedColour, vec3(gamma_inverse)), 1.0);
}