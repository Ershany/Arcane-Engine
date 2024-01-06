#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colour;

out vec3 LineColour;

uniform mat4 viewProjection;

void main()
{
	LineColour = colour;
	gl_Position = viewProjection * vec4(position, 1.0);
}




#shader-type fragment
#version 430 core

in vec3 LineColour;

out vec4 FragColour;

void main()
{
	FragColour = vec4(LineColour, 1.0);
}
