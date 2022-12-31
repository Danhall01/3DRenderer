#pragma once
#include <DirectXMath.h>
#include "Structures.h"

// A simple FPS free-float camera
class Camera
{
public:
	Camera();
	Camera(const dx::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ);
	Camera(float right, float up, float forward, float fovDegrees, float aspectRatio, float nearZ, float farZ);
	Camera(
		float right, float up, float forward,
		float eulerPitch, float eulerYaw, float eulerRoll,
		float fovDegrees, float aspectRatio, float nearZ, float farZ);
	Camera(
		float right, float up, float forward,
		float x, float y, float z,
		float fovDegrees, float aspectRatio, float nearZ, float farZ,
		const lightType type);
	~Camera() = default;

	Camera operator=(const Camera& otherCam);

	const dx::XMMATRIX& GetViewMatrix() const;
	const dx::XMMATRIX& GetProjectionMatrix() const;

	const dx::XMFLOAT3& GetPositionFloat3() const;
	const dx::XMFLOAT3& GetRotationFloat3() const;


	void UpdateProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void UpdateProjectionMatrixOrthographic(float viewWidth, float viewHeight, float nearZ, float farZ);
	void UpdateViewMatrix();
	void UpdateViewMatrixRoll();

	void AddPosition(float right, float up, float forward);
	void AddRotation(float pitch, float yaw, float roll);
	void AddRotation(float pitch, float yaw, float roll, float deltaTime);

private:

private:
	dx::XMMATRIX m_viewMatrix;
	dx::XMMATRIX m_projectionMatrix;

	dx::XMFLOAT3 m_position;
	// Pitch Yaw Roll
	dx::XMFLOAT3 m_rotation;


	const dx::XMVECTOR DEFAULT_UP  = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const dx::XMVECTOR DEFAULT_FWD = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	static constexpr float ROTATION_SPEED = 0.004f;
	static constexpr float TRAVEL_SPEED = 0.13f;
	
};