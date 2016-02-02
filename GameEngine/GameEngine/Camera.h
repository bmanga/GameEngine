#pragma once

#include "math.h"

namespace Lemur
{
	class Camera
	{
		friend dVector3d getdVector3dInCameraSpace(Camera& camera, dVector3d vector);
		friend dVector3d getdVector3dInWorldSpace(Camera& camera, dVector3d vector);
	public:
		Camera() = default;
		~Camera() = default;

		void setOrigin(double x, double y, double z);
		void setTarget(double x, double y, double z);
	
		void rotateLocalX(double angle);
		void rotateLocalY(double angle);
		void rotateLocalZ(double angle);

		void update();

	private:
		dVector3d m_eye {0, 0, 0};
		dVector3d m_target {0, 0, 0};
		dVector3d m_up{ 0.0, 1.0, 0.0 };
	};
	
}
