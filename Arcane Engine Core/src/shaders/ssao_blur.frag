#version 430 core

in vec2 TexCoords;

out float FragColour;

uniform sampler2D ssaoInput;

uniform int numSamplesAroundTexel; // Represents the amount of texels around the center texel (2 = 5x5 matrix, 3 = 7x7, 4 = 9x9)

void main() {
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));

	float totalResult = 0.0f;
	for (int y = -numSamplesAroundTexel; y <= numSamplesAroundTexel; ++y) {
		for (int x = -numSamplesAroundTexel; x <= numSamplesAroundTexel; ++x) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			totalResult += texture(ssaoInput, TexCoords + offset).r;
		}
	}

	float kernelRowColSize = (numSamplesAroundTexel + numSamplesAroundTexel) + 1;
	FragColour = totalResult / (kernelRowColSize * kernelRowColSize);
}
