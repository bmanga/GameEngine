#version 140

/*
INSTRUCTIONS:

Follows the principle of an uber-shader in that all of the functionality is already present
in the code. ifdefs are used to select which functionality should be present in the shader
code.

DEFINES:

NUM_DIR_LIGHTS <value> - The number of directional light sources.
NUM_POINT_LIGHTS <value> - The number of point light sources.
NUM_SPOT_LIGHTS <value> - The number of spotlight sources.
USE_TEXTURES - Specifies that lighting calculations should use textures.
*/

struct Material
{
#ifdef USE_TEXTURES
	sampler2D texture;
	sampler2D diffuse_map;
	sampler2D emissive_map;
	sampler2D normal_map;
	sampler2D specular_map;
	sampler2D mask_map;
#else
	vec3 ambient;
	vec3 diffuse;
	vec3 emissive;
	vec3 specular;
#endif

	float shininess;
	float transparency;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
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

in vec3 color;
in vec2 texcoord;
in vec3 normal;
in vec3 frag_pos;

out vec4 out_color;

uniform vec3 view_pos;
uniform Material material;
#ifdef NUM_DIR_LIGHTS
uniform DirLight dir_light;
#endif
#ifdef NUM_POINT_LIGHTS
uniform PointLight point_lights[NUM_POINT_LIGHTS];
#endif
#ifdef NUM_SPOT_LIGHTS
uniform SpotLight spot_light;
#endif

vec3 calcDirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
	vec3 result;
	vec3 norm;
	vec3 view_dir = normalize(view_pos - frag_pos);

#ifdef USE_TEXTURES
	norm = vec3(texture(material.normal_map, texcoord));
#else
	norm = normalize(normal);
#endif

#ifdef NUM_DIR_LIGHTS
	// Phase 1: Directional lighting
	result = calcDirLight(dir_light, norm, view_dir);
#endif

#ifdef NUM_POINT_LIGHTS
	// Phase 2: Point lighting
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		result += calcPointLight(point_lights[i], norm, frag_pos, view_dir);
	}
#endif

#ifdef NUM_SPOT_LIGHTS
	// Phase 3: Spotlight lighting
	result += calcSpotLight(spot_light, norm, frag_pos, view_dir);
#endif

	// Emissive (TODO: Confirm this calculation is correct)
#ifdef USE_TEXTURES
	result += vec3(texture(material.emissive_map, texcoord));
#else
	result += material.emissive;
#endif

	//result *= texture2D(texture, texcoord);
	out_color = vec4(result, 1.0f - material.transparency);
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
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec;

#ifdef USE_TEXTURES
	ambient *= vec3(texture(material.diffuse_map, texcoord));
	diffuse *= vec3(texture(material.diffuse_map, texcoord));
	specular *= vec3(texture(material.specular_map, texcoord));
#else
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
#endif

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
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec;

#ifdef USE_TEXTURES
	ambient *= vec3(texture(material.diffuse_map, texcoord));
	diffuse *= vec3(texture(material.diffuse_map, texcoord));
	specular *= vec3(texture(material.specular_map, texcoord));
#else
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
#endif

	// Attenuate colors based on distance
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
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec;

#ifdef USE_TEXTURES
	ambient *= vec3(texture(material.diffuse_map, texcoord));
	diffuse *= vec3(texture(material.diffuse_map, texcoord));
	specular *= vec3(texture(material.specular_map, texcoord));
#else
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
#endif

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}