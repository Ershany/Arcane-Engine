#version 430 core

out vec4 FragColour;

in vec2 TexCoord;

struct Material {
	sampler2D texture_diffuse;
};

uniform Material material;
uniform vec2 readOffset;

void main() {
	// Sample the fragments around the current fragment for post processing using kernels (convulution matrices)
	vec2 readOffsets[9] = vec2[] (
		vec2(-readOffset.x, readOffset.y),
		vec2(0.0, readOffset.y),
		vec2(readOffset.x, readOffset.y),
		vec2(-readOffset.x, 0.0),
		vec2(0.0, 0.0),
		vec2(readOffset.x, 0.0),
		vec2(-readOffset.x, -readOffset.y),
		vec2(0.0, -readOffset.y),
		vec2(readOffset.x, -readOffset.y)
	);
	
	// Note: Post processing may cause aliasing since it uses the blitted framebuffer (A non-multisampled buffer)

	// Blur kernel
	//float kernel[9] = float[] (
	//	1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
	//	2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
	//	1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
	//);
	
	// Sharpen kernel
	//float kernel[9] = float[] (
	//	-1, -1, -1,
	//	-1,  9, -1,
	//	-1, -1, -1
	//);

	// Edge Highlight kernel
	//float kernel[9] = float[] (
	//	-2, -2, -2,
	//	-2, 15, -2,
	//	-2, -2, -2
	//);

	// No Post Processing Kernel
	float kernel[9] = float[] (
		0, 0, 0,
		0, 1, 0,
		0, 0, 0
	);

	vec3 colour = vec3(0.0);

	// Apply the kernel (post-processing effect)
	for(int i = 0; i < 9; ++i) {
		colour += texture(material.texture_diffuse, TexCoord + readOffsets[i]).rgb * kernel[i];
	}
	FragColour = vec4(colour, 1.0);
}