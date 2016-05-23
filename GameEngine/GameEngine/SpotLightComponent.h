#pragma once

#include "math.h"

struct CSpotLight
{
	glm::vec3 direction;

	float cut_off;
	float outer_cut_off;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};