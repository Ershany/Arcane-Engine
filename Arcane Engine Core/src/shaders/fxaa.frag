#version 430 core

#define FXAA_REDUCE_MIN (1.0 / 128.0)
#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_SPAN_MAX 8.0

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D colour_texture;
uniform vec2 inverse_resolution;
uniform int enable_FXAA;

void main() {
	// if FXAA is disabled early out
	vec3 rgbM  = texture2D(colour_texture, TexCoords).xyz;
	if (enable_FXAA == 0) {
		FragColour = vec4(rgbM, 1.0);
		return;
	}

	// Samples the texels around and calculate their corresponding luminosity
	vec3 calculateLuma = vec3(0.299, 0.587, 0.114);
	vec3 rgbNW = texture2D(colour_texture, TexCoords + (vec2(-1.0,-1.0)) * inverse_resolution).xyz;
	vec3 rgbNE = texture2D(colour_texture, TexCoords + (vec2(1.0,-1.0)) * inverse_resolution).xyz;
	vec3 rgbSW = texture2D(colour_texture, TexCoords + (vec2(-1.0,1.0)) * inverse_resolution).xyz;
	vec3 rgbSE = texture2D(colour_texture, TexCoords + (vec2(1.0,1.0)) * inverse_resolution).xyz;

	float lumaM  = dot(rgbM,  calculateLuma);
	float lumaNW = dot(rgbNW, calculateLuma);
	float lumaNE = dot(rgbNE, calculateLuma);
	float lumaSW = dot(rgbSW, calculateLuma);
	float lumaSE = dot(rgbSE, calculateLuma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE))); 

	// Calculate sample direction
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * inverse_resolution;

	// Perform the samples and calculate the new texel colour
	vec3 rgbA = 0.5 * (texture2D(colour_texture, TexCoords + dir * ((1.0 / 3.0) - 0.5)).xyz + texture2D(colour_texture, TexCoords + dir * ((2.0 / 3.0) - 0.5)).xyz);
	vec3 rgbB = rgbA * 0.5 + 0.25 * (texture2D(colour_texture, TexCoords + dir * - 0.5).xyz + texture2D(colour_texture, TexCoords + dir * 0.5).xyz);
	float lumaB = dot(rgbB, calculateLuma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
		FragColour = vec4(rgbA, 1.0);
	} else {
		FragColour = vec4(rgbB, 1.0);
	}
}
