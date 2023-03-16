#shader-type vertex
#version 430 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);
}




#shader-type fragment
#version 430 core

void main() {

}
