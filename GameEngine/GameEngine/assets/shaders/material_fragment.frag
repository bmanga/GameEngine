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

struct Material
{
	sampler2D texture;
	sampler2D diffuse_map;
	sampler2D emissive_map;
	sampler2D normal_map;
	sampler2D specular_map;
	sampler2D mask_map;

	float shininess;
	float transparency;
};

uniform sampler2D mysampler;
uniform vec3 view_pos;
uniform SpotLight spot_light;
uniform Material material;

in vec2 texcoord;
in vec3 frag_pos;


vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
	vec3 light_dir = normalize(view_pos - frag_pos);
	
	//Diffuse Shading
	float diff = max(dot(normal, light_dir), 0.0);
	
	//Specular shading
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
	
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}


void main (void)  
{  
	vec4 texture_color = texture2D(mysampler, texcoord);  
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 norm = vec3(1.0, 1.0, 1.0);
	
	vec3 result = calcSpotLight(spot_light, norm, frag_pos, view_dir);

	
	gl_FragColor = texture_color * vec4(result, 1);
}    
