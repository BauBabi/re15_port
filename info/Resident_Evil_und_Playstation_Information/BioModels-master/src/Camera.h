#ifndef CAMERA_H
#define CAMERA_H

#include "Math.h"

struct Camera
{
	Vec3 position;
	Vec3 front;
	Vec3 up;
	float zoom = 45.0f;

	inline Mat4 GetViewMatrix() const
	{
		return LookAtRH(position, position + front, up);
	}

	inline Mat4 GetPerspective(float aspectRatio, float zNear = 0.1f, float zFar = 100.0f) const
	{
		return PerspectiveRH(DEGTORAD(zoom), aspectRatio, zNear, zFar);
	}

	inline void MoveForward(float delta, float velocity)
	{
		float cameraSpeed = delta * velocity;
		position += front * cameraSpeed;
	}

	inline void MoveBackward(float delta, float velocity)
	{
		float cameraSpeed = delta * velocity;
		position -= front * cameraSpeed;
	}

	inline void MoveRight(float delta, float velocity)
	{
		float cameraSpeed = delta * velocity;
		position += front.cross(up).norm() * cameraSpeed;
	}

	inline void MoveLeft(float delta, float velocity)
	{
		float cameraSpeed = delta * velocity;
		position -= front.cross(up).norm() * cameraSpeed;
	}
};

#endif