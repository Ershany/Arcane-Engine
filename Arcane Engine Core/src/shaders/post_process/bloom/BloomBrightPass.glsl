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

uniform sampler2D scene_capture;

uniform float threshold;

void main() {
	vec3 hdrColour = texture2D(scene_capture, TexCoords).rgb;
	
	// Extract the bright areas
	if (dot(hdrColour, vec3(0.2126, 0.7152, 0.0722)) > threshold) {
		FragColour = vec4(hdrColour, 1.0);
	}
	else {
		FragColour = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
