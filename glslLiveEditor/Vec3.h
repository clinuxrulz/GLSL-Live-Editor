#ifndef _VEC3_H_
#define _VEC3_H_

#include <math.h>

class Vec3
{
private:
	const double x, y, z;
public:
	Vec3(const Vec3& v): x(v.x), y(v.y), z(v.z) {}
	Vec3(double x, double y, double z): x(x), y(y), z(z) {}

	Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
	Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
	Vec3 operator*(double a) const { return Vec3(x * a, y * a, z * a); }

	Vec3 cross(const Vec3& v) const {
		return Vec3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}

	Vec3 normalize() const {
		return *this * (1.0 / getLength());
	}

	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	double dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

	double getLengthSquared() const { return this->dot(*this); }
	double getLength() const { return sqrt(getLengthSquared()); }
};

#endif
