#include "Camera.h"
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

static double degreesToRadians(double degrees) {
	return degrees * M_PI / 180.0;
}

void Camera::moveLeft(double distance) {
	setPosition(*position + up->cross(*forward) * distance);
}

void Camera::moveRight(double distance) {
	setPosition(*position + forward->cross(*up) * distance);
}

void Camera::moveUp(double distance) {
	setPosition(*position + *up * distance);
}

void Camera::moveDown(double distance) {
	setPosition(*position + *up * -distance);
}

void Camera::moveForward(double distance) {
	setPosition(*position + *forward * distance);
}

void Camera::moveBack(double distance) {
	setPosition(*position + *forward * -distance);
}

void Camera::turnUp(double deltaAngle) {
	turnDown(-deltaAngle);
}

void Camera::turnDown(double deltaAngle) {
	deltaAngle = degreesToRadians(deltaAngle);
	double ca = cos(deltaAngle);
	double sa = sin(deltaAngle);
	setForward(*forward * ca - *up * sa);
	setUp(*up * ca + *forward * sa);
	orthonormalize();
}

void Camera::turnRight(double deltaAngle) {
	turnLeft(-deltaAngle);
}

void Camera::turnLeft(double deltaAngle) {
	deltaAngle = degreesToRadians(deltaAngle);
	double ca = cos(deltaAngle);
	double sa = sin(deltaAngle);
	Vec3 right = forward->cross(*up);
	setForward(*forward * ca - right * sa);
	orthonormalize();
}

void Camera::orthonormalize() {
	Vec3 right = forward->cross(*up);
	setForward(up->cross(right));
	setUp(up->normalize());
	setForward(forward->normalize());
}
