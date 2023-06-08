#ifndef CAMERA_H
#define CAMERA_H

#include <vmath.h>

#define MAX_FOV 120.f
#define MIN_FOV 10.f

enum MoveDirection {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	vmath::vec3 eye, center, up, worldUp;
	float fov;
	float yaw, pitch;

	Camera(vmath::vec3 _eye = vmath::vec3(0.f, 0.f, 1.0f),
		vmath::vec3 _up = vmath::vec3(0.f, 1.f, 0.f), float _fov = 50.f,
		float _yaw = -90.f, float _pitch = 0)
	{
		eye = _eye;
		worldUp = up = _up;

		fov = _fov;

		yaw = _yaw;
		pitch = _pitch;

		computeCameraParameters();
	}

	vmath::mat4 lookat()
	{
		return vmath::lookat(eye, center, up);
	}

	vmath::mat4 perspective(int windowWidth, int windowHeight, float nearPlane = 0.1f, float farPlane = 1000.f)
	{
		return  vmath::perspective(fov, (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
	}

	void onMouseWheel(int pos)
	{
		if (pos > 0)
			fov = vmath::min(MAX_FOV, fov + 1.0f);
		else
			fov = vmath::max(MIN_FOV, fov - 1.0f);
	}

	void moveAlongDirection(MoveDirection direction)
	{
		vmath::vec3 frontVector = vmath::normalize(center - eye);
		vmath::vec3 rightVector = vmath::normalize(vmath::cross(frontVector, up));
		vmath::vec3 upVector = vmath::normalize(vmath::cross(rightVector, frontVector));

		float movingStep = 0.1f;

		if (direction == FORWARD)
			eye += frontVector * movingStep;
		if (direction == BACKWARD)
			eye -= frontVector * movingStep;
		if (direction == RIGHT)
			eye += rightVector * movingStep;
		if (direction == LEFT)
			eye -= rightVector * movingStep;

		center = eye + frontVector;
	}

	void rotate(float yawOffset, float pitchOffset)
	{
		yaw += yawOffset * 0.1f;
		pitch -= pitchOffset * 0.1f;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		computeCameraParameters();
	}

	void computeCameraParameters()
	{
		vmath::vec3 frontVector;

		frontVector[0] = cos(vmath::radians(yaw)) * cos(vmath::radians(pitch));
		frontVector[1] = sin(vmath::radians(pitch));
		frontVector[2] = sin(vmath::radians(yaw)) * cos(vmath::radians(pitch));

		center = frontVector + eye;
		vmath::vec3 right = vmath::normalize(vmath::cross(frontVector, worldUp));
		up = vmath::normalize(vmath::cross(right, frontVector));
	}

};


#endif 