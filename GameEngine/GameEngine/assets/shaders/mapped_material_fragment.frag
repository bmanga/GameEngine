#version 140

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cut_off;
	float outer_cut_off;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 color;
in vec2 texcoord;
in vec3 normal;
in vec3 frag_pos;

out vec4 out_color;

uniform sampler2D tex;
uniform vec3 view_pos;
uniform Material material;
uniform DirLight dir_light;
uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform SpotLight spot_light;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
	vec3 norm = normalize(normal);
	vec3 view_dir = normalize(view_pos - frag_pos);

	// Phase 1: Direction lighting
	vec3 result = calcDirLight(dir_light, norm, view_dir);

	// Phase 2: Point lighting
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += calcPointLight(point_lights[i], norm, frag_pos, view_dir);
	}

	// Phase 3: Spot lighting
	result += calcSpotLight(spot_light, norm, frag_pos, view_dir);

	// Apply per-vertex coloring
	result *= color;

	out_color = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 view_dir)
{
	vec3 light_dir = normalize(-light.direction);

	// Diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);

	// Specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

	// Combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texcoord));

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
	vec3 light_dir = normalize(light.position - frag_pos);

	// Diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);

	// Specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

	// Attenuation
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texcoord));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
	vec3 light_dir = normalize(light.position - frag_pos);

	// Diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);

	// Specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

	// Attenuation
	float distance = length(light.position - frag_pos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Spotlight intensity
	float theta = dot(light_dir, normalize(-light.direction));
	float epsilon = light.cut_off - light.outer_cut_off;
	float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

	// Combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texcoord));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}