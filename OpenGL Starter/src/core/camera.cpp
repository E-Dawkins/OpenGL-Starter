#include "pch.h"

#include "core/camera.h"

Camera::Camera(const glm::vec3& _location, float _aspectRatio, float _yaw, float _pitch)
	: position(_location), mAspectRatio(_aspectRatio), mYaw(_yaw), mPitch(_pitch)
{
	UpdateCameraVectors();
	UpdateProjectionMatrix();
}

void Camera::Update(GLFWwindow* /*_window*/, float /*_deltaTime*/)
{
	// base class does nothing
}

void Camera::AddYaw(float _yawOffset)
{
	if (_yawOffset != 0.f)
	{
		mYaw += _yawOffset;
		UpdateCameraVectors();
	}
}

void Camera::AddPitch(float _pitchOffset, bool _clampPitch)
{
	if (_pitchOffset != 0.f)
	{
		mPitch += _pitchOffset;

		if (_clampPitch)
			mPitch = glm::clamp(mPitch, -89.f, 89.f);

		UpdateCameraVectors();
	}
}

void Camera::FaceLocation(const glm::vec3& _location)
{
	if (position == _location) // can't face our own location
	{
		return;
	}

	glm::vec3 direction = glm::normalize(_location - position);

	// assumes world up is +Y, and world forward is +Z
	mPitch = glm::degrees(asin(direction.y));
	mYaw = glm::degrees(atan2(direction.z, direction.x));

	UpdateCameraVectors();
}

const glm::vec3& Camera::GetCameraUp() const
{
	return mUp;
}

const glm::vec3& Camera::GetCameraRight() const
{
	return mRight;
}

const glm::vec3& Camera::GetCameraForward() const
{
	return mForward;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(position, position + mForward, mUp);
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}

glm::mat4 Camera::GetViewProjectionMatrix()
{
	return GetProjectionMatrix() * GetViewMatrix();
}

void Camera::SetNearClipPlane(float _near)
{
	if (mNear != _near)
	{
		mNear = _near;
		UpdateProjectionMatrix();
	}
}

void Camera::SetFarClipPlane(float _far)
{
	if (mFar != _far)
	{
		mFar = _far;
		UpdateProjectionMatrix();
	}
}

void Camera::SetFov(float _fovDegrees)
{
	if (mFovDegrees != _fovDegrees)
	{
		mFovDegrees = _fovDegrees;
		UpdateProjectionMatrix();
	}
}

void Camera::SetAspectRatio(float _aspectRatio)
{
	if (mAspectRatio != _aspectRatio)
	{
		mAspectRatio = _aspectRatio;
		UpdateProjectionMatrix();
	}
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 newForward;
	newForward.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	newForward.y = sin(glm::radians(mPitch));
	newForward.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));

	mForward = glm::normalize(newForward);
	mRight = glm::normalize(glm::cross(mForward, glm::vec3(0, 1, 0))); // assuming world up is +Y
	mUp = glm::normalize(glm::cross(mRight, mForward));
}

void Camera::UpdateProjectionMatrix()
{
	mProjectionMatrix = glm::perspective(glm::radians(mFovDegrees), mAspectRatio, mNear, mFar);
}
