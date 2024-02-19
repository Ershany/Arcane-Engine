#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(position, 1.0);
	TexCoords = texCoord;
}




#shader-type fragment
#version 430 core

in vec2 TexCoords;

out vec4 FragColour;

uniform vec2 texelSize;
uniform sampler2D textureToDownsample;


// Better, temporally stable box filtering
// [Jimenez14] http://goo.gl/eomGso
// . . . . . . .
// . A . B . C .
// . . D . E . .
// . F . G . H .
// . . I . J . .
// . K . L . M .
// . . . . . . .
// Taking bilinear samples to reduce # of samples needed. AKA Kawase downsample
// But taking 13 taps instead, this was found to be temporally more stable by Sledgehammer Games
vec3 DownsampleBox13Tap(sampler2D tex, vec2 uv, vec2 texelSize)
{
	vec3 A = texture(tex, uv + texelSize * vec2(-1.0, -1.0)).rgb;
	vec3 B = texture(tex, uv + texelSize * vec2( 0.0, -1.0)).rgb;
	vec3 C = texture(tex, uv + texelSize * vec2( 1.0, -1.0)).rgb;
	vec3 D = texture(tex, uv + texelSize * vec2(-0.5, -0.5)).rgb;
	vec3 E = texture(tex, uv + texelSize * vec2( 0.5, -0.5)).rgb;
	vec3 F = texture(tex, uv + texelSize * vec2(-1.0,  0.0)).rgb;
	vec3 G = texture(tex, uv                               ).rgb;
	vec3 H = texture(tex, uv + texelSize * vec2( 1.0,  0.0)).rgb;
	vec3 I = texture(tex, uv + texelSize * vec2(-0.5,  0.5)).rgb;
	vec3 J = texture(tex, uv + texelSize * vec2( 0.5,  0.5)).rgb;
	vec3 K = texture(tex, uv + texelSize * vec2(-1.0,  1.0)).rgb;
	vec3 L = texture(tex, uv + texelSize * vec2( 0.0,  1.0)).rgb;
	vec3 M = texture(tex, uv + texelSize * vec2( 1.0,  1.0)).rgb;

	// Apply weight distribution from Call of Duty Advanced Warfare presentation, here is an energy conserving optimized way of doing it from : https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
	vec3 downsampleResult = G * 0.125;
	downsampleResult += (A + C + K + M) * 0.03125;
	downsampleResult += (B + F + H + L) * 0.0625;
	downsampleResult += (D + E + I + J) * 0.125;

	/*
	vec3 A = texture(tex, uv + texelSize * vec2(-1.0, -1.0)).rgb;
	vec3 B = texture(tex, uv + texelSize * vec2( 0.0, -1.0)).rgb;
	vec3 C = texture(tex, uv + texelSize * vec2( 1.0, -1.0)).rgb;
	vec3 D = texture(tex, uv + texelSize * vec2(-0.5, -0.5)).rgb;
	vec3 E = texture(tex, uv + texelSize * vec2( 0.5, -0.5)).rgb;
	vec3 F = texture(tex, uv + texelSize * vec2(-1.0,  0.0)).rgb;
	vec3 G = texture(tex, uv                               ).rgb;
	vec3 H = texture(tex, uv + texelSize * vec2( 1.0,  0.0)).rgb;
	vec3 I = texture(tex, uv + texelSize * vec2(-0.5,  0.5)).rgb;
	vec3 J = texture(tex, uv + texelSize * vec2( 0.5,  0.5)).rgb;
	vec3 K = texture(tex, uv + texelSize * vec2(-1.0,  1.0)).rgb;
	vec3 L = texture(tex, uv + texelSize * vec2( 0.0,  1.0)).rgb;
	vec3 M = texture(tex, uv + texelSize * vec2( 1.0,  1.0)).rgb;

	vec2 divisionWeights = (1.0 / 4.0) * vec2(0.5, 0.125); // 2 weights, 0.5 for middle 4, and 0.125 for the other 4 outside squares. Multiply weight by 1.0/4.0 to average the 4 samples per tap

	vec3 downsampleResult = (D + E + I + J) * divisionWeights.x;
	downsampleResult += (A + B + F + G) * divisionWeights.y;
	downsampleResult += (B + C + G + H) * divisionWeights.y;
	downsampleResult += (F + G + K + L) * divisionWeights.y;
	downsampleResult += (G + H + L + M) * divisionWeights.y;
	*/

	return downsampleResult;
}

void main()
{
	vec3 downsampleBlurResult = DownsampleBox13Tap(textureToDownsample, TexCoords, texelSize);
	FragColour = vec4(downsampleBlurResult, 1.0);
}
