#pragma once

#include "math.h"

class Transform
{
private:
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::quat orientation;

public:
	Transform()
	{

	}

	void setTranslation(float x, float y, float z)
	{
		translation = { x, y, z };
	}

	void setRotation(float x, float y, float z)
	{
		rotation = { x, y, z };
	}
	
	void setScale(float x, float y, float z)
	{
		scale = { x, y, z };
	}

	void setOrientation(float x, float y, float z, float w)
	{
		orientation = { x, y, z, w };
	}
};