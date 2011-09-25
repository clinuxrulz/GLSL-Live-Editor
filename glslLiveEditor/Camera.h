#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vec3.h"

class Camera {
public:
	Camera() {
		depth = 400.0;
		position.set(0.0, 60.0, 60.0);
		up.set(0.0, 1.0, 0.0);
		forward.set(0.0, 0.0, -1.0);
	}

	double getDepth() const { return depth; }
	vec3 getPosition() const { return position; }
	vec3 getUp() const { return up; }
	vec3 getForward() const { return forward; }

	void setDepth(double depth) { this->depth = depth; }
	void setPosition(const vec3& position) { this->position = position; }
	void setUp(const vec3& up) { this->up = up; }
	void setForward(const vec3& forward) { this->forward = forward; }

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
	vec3 position;
	vec3 up;
	vec3 forward;
};

#endif // _CAMERA_H_
