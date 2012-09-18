#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vec3.h"

class Camera {
public:
	Camera() {
		depth = 400.0;
		position = new Vec3(0, 60, 0);
		up = new Vec3(0, 1, 0);
		forward = new Vec3(0, 0, -1);
	}

	~Camera() {
		delete position;
		delete up;
		delete forward;
	}

	double getDepth() const { return depth; }
	Vec3 getPosition() const { return *position; }
	Vec3 getUp() const { return *up; }
	Vec3 getForward() const { return *forward; }

	void setDepth(double depth) { this->depth = depth; }
	void setPosition(const Vec3& position) { delete this->position; this->position = new Vec3(position); }
	void setUp(const Vec3& up) { delete this->up; this->up = new Vec3(up); }
	void setForward(const Vec3& forward) { delete this->forward; this->forward = new Vec3(forward); }

	void moveLeft(double distance);
	void moveRight(double distance);
	void moveUp(double distance);
	void moveDown(double distance);
	void moveForward(double distance);
	void moveBack(double distance);
	void turnUp(double deltaAngle);
	void turnDown(double deltaAngle);
	void turnRight(double deltaAngle);
	void turnLeft(double deltaAngle);

private:
	void orthonormalize();

	double depth;
	Vec3* position;
	Vec3* up;
	Vec3* forward;
};

#endif // _CAMERA_H_
