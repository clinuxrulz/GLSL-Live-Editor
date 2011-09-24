#ifndef _VEC3_H_
#define _VEC3_H_

#include <math.h>

class vec3 {
public:
	vec3() {
		x = y = z = 0.0;
	}

	vec3(const vec3& v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	vec3(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	vec3& operator=(const vec3& rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		return *this;
	}

	vec3& operator+=(const vec3& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	vec3& operator-=(const vec3& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	vec3& operator*=(double a) {
		this->x *= a;
		this->y *= a;
		this->z *= a;
		return *this;
	}

	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	void setX(double x) { this->x = x; }
	void setY(double y) { this->y = y; }
	void setZ(double z) { this->z = z; }

	void set(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	double dot(const vec3& rhs) {
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	void cross(const vec3& lhs, const vec3& rhs) {
		set(lhs.y * rhs.z - lhs.z * rhs.y,
		    lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x);
	}

	double getLengthSquared() {
		return this->dot(*this);
	}

	double getLength() {
		return sqrt(getLengthSquared());
	}

	void normalize() {
		*this *= 1.0 / getLength();
	}
private:
	double x, y, z;
};

#endif // _VEC3_H_
