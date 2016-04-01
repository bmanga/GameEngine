#include <GL/glew.h>

#include <gl/GL.h>
#include <gl/GLU.h>
#include "Camera.h"

#include "math.h"


using namespace Lemur;

Camera::Camera() 
	
{
	m_perspective = (glm::perspective(m_fov, m_aspect, m_zNear, m_zFar));
}

void Camera::rotateLocalX(float degrees)
{
	auto x_vect = glm::cross(m_direction, m_up);

	glm::quat A = { 0, m_direction.x, m_direction.y, m_direction.z };
	glm::quat B = { 0, x_vect.x, x_vect.y, x_vect.z };
	auto u = glm::normalize(B) * sin(degrees * 3.14f / 360);
	
	glm::quat q = { cos(degrees* 3.14f / 360), u.x, u.y, u.z };

	auto res = q * A * glm::inverse(q);

	m_direction = { res.x, res.y, res.z };
	
	m_up = glm::normalize(glm::cross(x_vect, m_direction));
	
}

void Camera::rotateLocalY(float degrees)
{
	glm::quat A = { 0, m_direction.x, m_direction.y, m_direction.z };
	glm::quat B = { 0, m_up.x, m_up.y, m_up.z };
	auto u = glm::normalize(B) * sin(degrees * 3.14f / 360);

	glm::quat q = { cos(degrees* 3.14f / 360), u.x, u.y, u.z };

	auto res = q * A * glm::inverse(q);

	m_direction = { res.x, res.y, res.z };
}

void Camera::rotateLocalZ(float degrees)
{
	glm::quat A = { 0, m_up.x, m_up.y, m_up.z };
	glm::quat B = { 0, m_direction.x, m_direction.y, m_direction.z };

	auto u = glm::normalize(B) * sin(degrees * 3.14f / 360);

	glm::quat q = { cos(degrees* 3.14f / 360), u.x, u.y, u.z };

	auto res = q * A * glm::inverse(q);

	m_up = { res.x, res.y, res.z };
}

void Camera::rotateRelativeX(float degrees)
{
	auto x_vect = glm::cross(m_direction, {0.0, 0.0, 1.0});

	glm::quat A = { 0, m_direction.x, m_direction.y, m_direction.z };
	glm::quat B = { 0, x_vect.x, x_vect.y, x_vect.z };
	auto u = glm::normalize(B) * sin(degrees * 3.14f / 360);

	glm::quat q = { cos(degrees* 3.14f / 360), u.x, u.y, u.z };

	auto res = q * A * glm::inverse(q);

	m_direction = { res.x, res.y, res.z };

	//TODO: may have to work more on this
	//m_up = glm::normalize(glm::cross(x_vect, m_direction));
}

void Camera::rotateRelativeY(float degrees)
{
	glm::quat A = { 0, m_direction.x, m_direction.y, m_direction.z };
	glm::quat B = { 0, 0.0, 0.0, 1.0 };
	auto u = glm::normalize(B) * sin(degrees * 3.14f / 360);

	glm::quat q = { cos(degrees* 3.14f / 360), u.x, u.y, u.z };

	auto res = q * A * glm::inverse(q);

	m_direction = { res.x, res.y, res.z };
}
