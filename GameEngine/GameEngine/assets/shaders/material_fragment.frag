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


#include <SpotLight.shpp>


void main (void)  
{  
	vec4 texture_color = texture2D(mysampler, texcoord);  
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 norm = vec3(1.0, 1.0, 1.0);
	
	vec3 result = calcSpotLight(spot_light, norm, frag_pos, view_dir);

	
	gl_FragColor = texture_color.rgba * vec4(result, 1); 
}    
