#version 330

in vec3 position;
in vec2 in_texcoord;
in vec3 in_normal;

out vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() 
{
	texcoord = vec2(in_texcoord.x, 1.0 - in_texcoord.y);
	gl_Position = proj * view * model * vec4(position, 1.0f);
}