#pragma once

#include "math.h"

namespace Lemur
{
using namespace math;
class Camera
{
public:
	Camera();
	~Camera() = default;

	mat4 Camera::getViewProjection() const
	{
		return m_perspective * glm::lookAt(m_eye, m_eye + m_direction, m_up);
	}

	// sorry bruno
	mat4 Camera::getView() const
	{
		return glm::lookAt(m_eye, m_eye + m_direction, m_up);
	}

	// sorry bruno pt 2
	mat4 Camera::getProjection() const
	{
		return m_perspective;
	}

	// sorry bruno pt 3 (but this should have already been here)
	vec3 Camera::getEye() const
	{
		return m_eye;
	}

	void translateLocalX(float step)
	{
		auto x_vect = glm::cross(m_direction, m_up);
		m_eye += x_vect * step;
	}
	
	void translateLocalY(float step)
	{
		m_eye += m_up * step;
	}

	void translateLocalZ(float step)
	{
		m_eye += (m_direction)* step;
	}

	void rotateLocalX(float degrees);

	void rotateLocalY(float degrees);

	void rotateLocalZ(float degrees);

	void rotateRelativeX(float degrees);
	void rotateRelativeY(float degrees);
	void rotateRelativeZ(float degrees);

private:
	mat4 m_perspective;
	vec3 m_eye {0, 3, 0};
	vec3 m_direction {0, -1, 0.0};
	vec3 m_up{ 0.0, 0.0, 1.0 };

	float m_zNear = 0.01f;
	float m_zFar  = 1000.0f;
	float m_fov = glm::radians(45.0f);
	float m_aspect = 16.0f / 9.0f;
};
	
}

