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

uniform sampler2D sceneCapture;

uniform vec4 filterValues;

void main() {
	vec3 hdrColour = texture2D(sceneCapture, TexCoords).rgb;

	// Extract the bright areas by calculating brightness and checking it against the threshold
	// The soft threshold is from this source: https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom/
	float brightness = dot(hdrColour, vec3(0.2126, 0.7152, 0.0722));
	float soft = brightness - filterValues.y;
	soft = clamp(soft, 0, filterValues.z);
	soft = soft * soft * filterValues.w;
	float contribution = max(soft, brightness - filterValues.x);
	contribution /= max(brightness, 0.00001);
	FragColour = vec4(hdrColour * contribution, 1.0);
}
