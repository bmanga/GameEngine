#version 140
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
in vec3 color;
in vec2 texcoord;
in vec3 normal;
in vec3 frag_pos;
out vec4 out_color;
uniform sampler2D tex;
uniform vec3 view_pos;
uniform Material material;
uniform Light light;
void main() {
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texcoord));
	vec3 norm = normalize(normal);
	vec3 light_dir = normalize(light.position - frag_pos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texcoord));
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texcoord));
	vec3 result = (ambient + diffuse + specular) * color;
	out_color = /*texture(tex, texcoord) */ vec4(result, 1.0f);
}
