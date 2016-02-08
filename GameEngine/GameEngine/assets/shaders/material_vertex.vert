#version 140

in vec3 position;
in vec3 color;
in vec2 in_texcoord;
in vec3 in_normal;

out vec3 vertex_color;
out vec2 texcoord;
out vec3 normal;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	vertex_color = color;
	texcoord = in_texcoord;
	normal = mat3(transpose(inverse(model))) * in_normal;

	frag_pos = vec3(model * vec4(position, 1.0f));
	gl_Position = proj * view * model * vec4(position, 1.0f);
}