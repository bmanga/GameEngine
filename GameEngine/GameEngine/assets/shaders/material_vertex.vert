#version 330

in vec3 position;
in vec2 in_texcoord;
in vec3 in_normal;

out vec2 texcoord;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() 
{
	frag_pos = vec3(model * vec4(position, 1.0f));
	texcoord = vec2(in_texcoord.x, 1.0 - in_texcoord.y);
	gl_Position = proj * view * model * vec4(position, 1.0f);
}