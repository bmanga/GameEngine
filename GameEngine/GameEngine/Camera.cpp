#include <GL/glew.h>

#include <gl/GL.h>
#include <gl/GLU.h>
#include "Camera.h"

#include "math.h"


using namespace Lemur;

inline void Camera::setOrigin(double x, double y, double z)
{
	m_eye = { x, y, z };
}

inline void Camera::setTarget(double x, double y, double z)
{
	m_target = { x, y, z };
}

void Camera::rotateLocalX(double degrees)
{
	auto targInCamSpace = getdVector3dInCameraSpace(*this, m_target);
	auto rotated = rotatedVector2d({targInCamSpace.y, targInCamSpace.z}, degrees);

	m_target = getdVector3dInWorldSpace(*this, {targInCamSpace.x, rotated.x, rotated.y});
}

void Camera::rotateLocalY(double degrees)
{
	auto targInCamSpace = getdVector3dInCameraSpace(*this, m_target);
	auto rotated = rotatedVector2d({ targInCamSpace.x, targInCamSpace.z }, degrees);

	m_target = getdVector3dInWorldSpace(*this, { rotated.x, targInCamSpace.y, rotated.y });
}

void Camera::rotateLocalZ(double degrees)
{
	auto targInCamSpace = getdVector3dInCameraSpace(*this, m_target);
	auto rotated = rotatedVector2d({ targInCamSpace.x, targInCamSpace.y }, degrees);

	m_target = getdVector3dInWorldSpace(*this, { rotated.x, rotated.y, targInCamSpace.z });
}

void Camera::update()
{
	gluLookAt(m_eye.x, m_eye.y, m_eye.z, m_target.x, m_target.y, m_target.z, m_up.x, m_up.y, m_up.z);
}

dVector3d Lemur::getdVector3dInCameraSpace(Camera& camera, dVector3d vector)
{
	return vector - camera.m_eye;
}

dVector3d Lemur::getdVector3dInWorldSpace(Camera& camera, dVector3d vector)
{
	return vector + camera.m_eye;
}
