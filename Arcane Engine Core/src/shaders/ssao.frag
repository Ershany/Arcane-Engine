#version 430 core

in vec2 TexCoords;

out float FragColour;

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D texNoise;

uniform int numKernelSamples;
uniform vec3 samples[64];

uniform mat4 projection;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(0.0, 0.0);

void main() {
	FragColour = 1.0;
}
