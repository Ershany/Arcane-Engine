#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0);
	TexCoords = texCoord;
}




#shader-type fragment
#version 430 core 

in vec2 TexCoords;

out vec4 FragColour;

uniform sampler2D sprite;

void main() {
	vec4 spriteColour = texture(sprite, TexCoords);
	if (spriteColour.a == 0)
		discard;

	FragColour = spriteColour;
}
