#include "Camera.h"
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

static double degreesToRadians(double degrees) {
	return degrees * M_PI / 180.0;
}

void Camera::moveLeft(double distance) {
	vec3 left;
	left.cross(up, forward);
	left *= distance;
	position += left;
}

void Camera::moveRight(double distance) {
	vec3 right;
	right.cross(forward, up);
	right *= distance;
	position += right;
}

void Camera::moveUp(double distance) {
	position += (vec3(up) *= distance);
}

void Camera::moveDown(double distance) {
	position += (vec3(up) *= -distance);
}

void Camera::moveForward(double distance) {
	position += (vec3(forward) *= distance);
}

void Camera::moveBack(double distance) {
	position += (vec3(forward) *= -distance);
}

void Camera::turnUp(double deltaAngle) {
	turnDown(-deltaAngle);
}

void Camera::turnDown(double deltaAngle) {
	deltaAngle = degreesToRadians(deltaAngle);
	double ca = cos(deltaAngle);
	double sa = sin(deltaAngle);
	forward = ((vec3(forward) *= ca) -= (vec3(up) *= sa));
	up = ((vec3(up) *= ca) += (vec3(forward) *= sa));
	orthonormalize();
}

void Camera::turnRight(double deltaAngle) {
	turnLeft(-deltaAngle);
}

void Camera::turnLeft(double deltaAngle) {
	deltaAngle = degreesToRadians(deltaAngle);
	double ca = cos(deltaAngle);
	double sa = sin(deltaAngle);
	vec3 right;
	right.cross(forward, up);
	forward = ((vec3(forward) *= ca) -= (vec3(right) *= sa));
	orthonormalize();
}

void Camera::orthonormalize() {
	vec3 right;
	right.cross(forward, up);
	forward.cross(up, right);
	up.normalize();
	forward.normalize();
}
