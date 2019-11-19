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

uniform float gamma_inverse;
uniform float exposure;
uniform vec2 read_offset;

void main() {
	vec3 hdrColour = texture(scene_capture, TexCoords).rgb;
	
	// Apply a simple exposure tonemap (HDR -> SDR) (dark scenes should have higher exposures, while bright scenes should have lower exposures)
	vec3 tonemappedColour = vec3(1.0) - exp(exposure * -hdrColour);

	// Apply gamma correction (Linear -> Non-linear)
	FragColour = vec4(pow(tonemappedColour, vec3(gamma_inverse)), 1.0);
}
