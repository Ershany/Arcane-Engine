#version 430 core

// FXAA variable tuning
const float FXAA_EDGE_THRESHOLD = 0.125;
const float FXAA_EDGE_THRESHOLD_MIN = 0.0625;

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D screen_texture;
uniform vec2 read_offset;

// function prototypes
vec3 FxaaPass();
float FxaaLuma(vec3 rgb);

void main() {
	FragColour = vec4(FxaaPass(), 1.0);
}

vec3 FxaaPass() {
	/*
	 *	Local Contrast Check
	 *	Check surrounding texel's and their luminosity, if the difference between the local max and min luma (contrast) is lower
	 *	than a threshold proportional to the maximum luma, the the shader can early out (no visible aliasing)
	*/
	vec3 rgbN = texture(screen_texture, TexCoords + vec2(0.0,				read_offset.y	)).rgb;
	vec3 rgbW = texture(screen_texture, TexCoords + vec2(-read_offset.x,	0.0				)).rgb;
	vec3 rgbM = texture(screen_texture, TexCoords + vec2(0.0,				0.0				)).rgb;
	vec3 rgbE = texture(screen_texture, TexCoords + vec2(read_offset.x,		0.0				)).rgb;
	vec3 rgbS = texture(screen_texture, TexCoords + vec2(0.0,				-read_offset.y	)).rgb;
	float lumaN = FxaaLuma(rgbN);
	float lumaW = FxaaLuma(rgbW);
	float lumaM = FxaaLuma(rgbM);
	float lumaE = FxaaLuma(rgbE);
	float lumaS = FxaaLuma(rgbS);
	float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
	float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
	float range = rangeMax - rangeMin;
	if (range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD)) {
		return rgbM;
	}

	/*
	 *	Sub-pixel Aliasing Test
	 *	Used to detect sub-pixel aliasing. Pixel contrast is estimated as the absolute difference in pixel luma from a lowpass luma (computed as the average of the surrounding texels)
	 *	This ratio approaches 1.0 in the presence of single pixel dots, and otherwise begins to fall off towards 0.0 as more pixels contribute to an edge.
	*/

}

float FxaaLuma(vec3 rgb) {
	return (0.2126 * rgb.r) + (0.7152 * rgb.g) + (0.0722 * rgb.b);
}
