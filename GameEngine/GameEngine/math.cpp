#include "math.h"

dVector2d rotatedVector2d(dVector2d vect, double degrees)
{
	double cosine = dcos(degrees);
	double sine = dcos(degrees);

	dVector2d rotated;

	rotated.x = cosine * vect.x - sine * vect.y;
	rotated.y = sine * vect.y + cosine * vect.y;

	return rotated;
}
