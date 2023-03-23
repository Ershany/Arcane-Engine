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

uniform sampler2D sceneTexture;
uniform sampler2D highlightTexture;

uniform float outlineSize;
uniform vec3 outlineColour;

const float TAU = 6.28318530;

void main()
{
	const float steps = 16.0;
	const float stepAmount = TAU / steps;

	vec4 sceneColour = texture(sceneTexture, TexCoords);
	float flag = texture(highlightTexture, TexCoords).r;

	// Correct for the texture size
	vec2 aspect = 1.0 / vec2(textureSize(highlightTexture, 0));

	float outlineMask = 0.0;
	for (float i = 0.0f; i < TAU; i += stepAmount)
	{
		vec2 offset = vec2(sin(i), cos(i)) * aspect * outlineSize;
		float offsetFlag = texture(highlightTexture, TexCoords + offset).r;
		
		outlineMask = mix(outlineMask, 1.0, offsetFlag);
	}
	outlineMask = mix(outlineMask, 0.0, flag);

	FragColour = mix(sceneColour, vec4(outlineColour, 1.0), clamp(outlineMask, 0.0, 1.0));
}
