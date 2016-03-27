#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lemur { namespace math {


using namespace glm;

static_assert(sizeof(vec3) == 4 * 3, "");

inline double dtan(double angle)
{
	return std::tan(angle * 3.14 / 180);
}

inline double dsin(double degrees)
{
	return std::sin(degrees * 3.14 / 180);
}

inline double dcos(double degrees)
{
	return std::cos(degrees * 3.14 / 180);
}

inline double distance3d(double firstx, double firsty, double firstz,
	double secondx, double secondy, double secondz)
{
	return std::sqrt(
		std::pow(secondx - firstx, 2) +
		std::pow(secondy - firsty, 2) +
		std::pow(secondz - firstz, 2));
}

struct dVector2d
{
	double x;
	double y;
};

struct dVector3d
{
	double x;
	double y;
	double z;
};

inline dVector3d operator- (const dVector3d& a, const dVector3d& b)
{
	return{ a.x - b.x, a.y - b.y, a.z - b.z };
}
inline dVector3d operator+ (const dVector3d& a, const dVector3d& b)
{
	return{ a.x + b.x, a.y + b.y, a.z + b.z };
}

dVector2d rotatedVector2d(dVector2d vect, double degrees);

}//namespace math
}//namespace Lemur