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
uniform sampler2D textureToUpsample;
uniform vec4 sampleScale;


// [Jimenez14] http://goo.gl/eomGso
// 9 tap bilinear upsample (tent filter)
vec3 UpsampleTent(sampler2D tex, vec2 uv, vec2 texelSize, vec4 sampleScale)
{
	vec4 d = texelSize.xyxy * vec4(1.0, 1.0, -1.0, 0.0) * sampleScale;

	vec3 s;
	s =  texture(tex, uv - d.xy).rgb;
	s += texture(tex, uv - d.wy).rgb * 2.0;
	s += texture(tex, uv - d.zy).rgb;

	s += texture(tex, uv + d.zw).rgb * 2.0;
	s += texture(tex, uv       ).rgb * 4.0;
	s += texture(tex, uv + d.xw).rgb * 2.0;

	s += texture(tex, uv + d.zy).rgb;
	s += texture(tex, uv + d.wy).rgb * 2.0;
	s += texture(tex, uv + d.xy).rgb;

	return s * (1.0 / 16.0);
}

void main()
{
	vec3 upsampleBloomResult = UpsampleTent(textureToUpsample, TexCoords, texelSize, sampleScale);
	FragColour = vec4(upsampleBloomResult, 1.0);
}
