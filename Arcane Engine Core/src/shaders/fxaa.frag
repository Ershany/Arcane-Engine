#version 430 core

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D colour_texture;

uniform vec2 read_offset;
uniform int enableFXAA;
uniform int showEdges;

uniform float lumaThreshold;
uniform float mulReduction;
uniform float minReduction;
uniform float maxSpan;

void main() {
	vec3 rgbM = texture(colour_texture, TexCoords).rgb;

	if (enableFXAA == 0) {
		FragColour = vec4(rgbM, 1.0);
		return;
	}

	vec3 rgbNW = texture(colour_texture, TexCoords + vec2(-read_offset.x,	read_offset.y)).rgb;
	vec3 rgbNE = texture(colour_texture, TexCoords + vec2(read_offset.x,	read_offset.y)).rgb;
	vec3 rgbSW = texture(colour_texture, TexCoords + vec2(-read_offset.x,	-read_offset.y)).rgb;
	vec3 rgbSE = texture(colour_texture, TexCoords + vec2(read_offset.x,	-read_offset.y)).rgb;

	// https://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
	const vec3 lumaCalculation = vec3(0.299, 0.587, 0.114);

	// Calculate the perceived luminosity
	float lumaM = dot(rgbM, lumaCalculation);
	float lumaNW = dot(rgbNW, lumaCalculation);
	float lumaNE = dot(rgbNE, lumaCalculation);
	float lumaSW = dot(rgbSW, lumaCalculation);
	float lumaSE = dot(rgbSE, lumaCalculation);

	// Calculate the minimum and maximum luma
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	// If the contrast is lower than a maximum, do not perform AA
	if (lumaMax - lumaMin <= lumaMax * lumaThreshold) {
		FragColour = vec4(rgbM, 1.0);
		return;
	}


	// Perform sampling along the gradient
	vec2 sampleDir;
	sampleDir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	sampleDir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	// Sampling step does depend on the luma: The brighter the sampled texels, the smaller the final sampling step
	// This means that brighter areas are less blurred/more sharper than dark areas
	float samplingDirectionReduction = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * mulReduction, minReduction);
	float minSamplingDirectionFactor = 1.0 / (min(abs(sampleDir.x), abs(sampleDir.y)) + samplingDirectionReduction);

	// Calculate the final sampling direction vector by reducing and clamping to a range and adapt it to the texture size
	sampleDir = clamp(sampleDir * minSamplingDirectionFactor, vec2(-maxSpan), vec2(maxSpan)) * read_offset;


	// Inner samples
	vec3 rgbSampleNegative = texture(colour_texture, TexCoords + sampleDir * ((1.0 / 3.0) - 0.5)).rgb;
	vec3 rgbSamplePositive = texture(colour_texture, TexCoords + sampleDir * ((2.0 / 3.0) - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSampleNegative + rgbSamplePositive) * 0.5;

	// Outer samples on the tab
	vec3 rgbSampleNegativeOuter = texture(colour_texture, TexCoords + sampleDir * ((0.0/3.0) - 0.5)).rgb;
	vec3 rgbSamplePositiveOuter = texture(colour_texture, TexCoords + sampleDir * ((3.0/3.0) - 0.5)).rgb;
	
	vec3 rgbFourTab = (rgbSampleNegativeOuter + rgbSamplePositiveOuter) * 0.25 + rgbTwoTab * 0.5;

	// Calculate luma for checking against the minimum and maximum value
	float lumaFourTab = dot(rgbFourTab, lumaCalculation);

	// Are outer samples of the tab beyond the edge?
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax) {
		// Yes it is, so only use the two samples
		FragColour = vec4(rgbTwoTab, 1.0);
	}
	else {
		// No, so we can use all four samples
		FragColour = vec4(rgbFourTab, 1.0);
	}

	// Debug view
	if (showEdges != 0) {
		FragColour.r = 1.0;
	}
}
