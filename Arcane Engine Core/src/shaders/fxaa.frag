#version 430 core

out vec4 FragColour;

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;
uniform vec2 sampleOffset;

void main() {
	// First step is to determine which fragments are part of the aliased edge.
	// You can attempt to determine this by checking for the difference of luminosity with the current fragment and the fragments around it.
	// This isn't perfect because this might blur textures slightly as well
	
	//FragColour = vec4(colour, 1.0);
}