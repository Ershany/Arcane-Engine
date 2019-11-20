#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = texCoords;
	gl_Position = vec4(position, 1.0);
}




#shader-type fragment
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
