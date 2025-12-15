#pragma once

// modified from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

class Camera
{
public:
	glm::vec3 position;

protected:
	// local camera axes
	glm::vec3 mUp, mRight, mForward;

	// local euler angles
	float mYaw, mPitch;

	// other camera attributes
	float mNear = 0.1f, mFar = 1000.f;
	float mFovDegrees = 45.f;
	float mAspectRatio;

	glm::mat4 mProjectionMatrix = glm::mat4(1);

public:
	Camera(const glm::vec3& _location, float _aspectRatio, float _yaw = 0.f, float _pitch = 0.f);

	virtual void Update(GLFWwindow* _window, float _deltaTime);

	void AddYaw(float _yawOffset);
	void AddPitch(float _pitchOffset, bool _clampPitch = true);

	void FaceLocation(const glm::vec3& _location);

	// Getters

	const glm::vec3& GetCameraUp() const;
	const glm::vec3& GetCameraRight() const;
	const glm::vec3& GetCameraForward() const;

	glm::mat4 GetViewMatrix() const;
	const glm::mat4& GetProjectionMatrix() const;
	glm::mat4 GetViewProjectionMatrix();

	// Setters

	void SetNearClipPlane(float _near);
	void SetFarClipPlane(float _far);
	void SetFov(float _fovDegrees);
	void SetAspectRatio(float _aspectRatio);

protected:
	void UpdateCameraVectors();
	void UpdateProjectionMatrix();

};
