#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoords;

void main() {
	gl_Position = vec4(position, 1.0);
	TexCoords = texCoord;
}




#shader-type fragment
#version 430 core

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D input_texture;
uniform sampler2D vignette_mask;

uniform bool usesMask;
uniform vec3 colour;
uniform float intensity;

void main() {
	vec3 sceneColour = texture2D(input_texture, TexCoords).rgb;
	
	if (usesMask) {
		float vignetteOpacity = (1.0 - texture(vignette_mask, TexCoords).r) * intensity;
		FragColour = vec4(mix(sceneColour, colour, vignetteOpacity), 1.0);
	}
	else {
		vec2 uv = TexCoords;
		uv *= 1.0 - TexCoords.xy;
		float vig = uv.x * uv.y * 15.0;
		vig = pow(vig, intensity * 5.0);

		FragColour = vec4(mix(colour, sceneColour, vig), 1.0);
	}
}
