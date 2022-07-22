#include <algorithm>
#include "Camera.h"

Camera::Camera()
{
	dx::XMStoreFloat3(&m_position, {0.0f,0.0f,0.0f});
	m_rotation = { 0.0f, 0.0f, 0.0f };
	UpdateProjectionMatrix(90, 1.8f, 0.1f, 100.0f);
	UpdateViewMatrix();
}
Camera::Camera(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	dx::XMStoreFloat3(&m_position, pos);
	m_rotation = { 0.0f, 0.0f, 0.0f };
	UpdateProjectionMatrix(fovDegrees, aspectRatio, nearZ, farZ);
	UpdateViewMatrix();
}
Camera::Camera(float right, float up, float forward, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_position = { right, up, forward };
	m_rotation = { 0.0f, 0.0f, 0.0f };
	UpdateProjectionMatrix(fovDegrees, aspectRatio, nearZ, farZ);
	UpdateViewMatrix();
}

Camera Camera::operator=(const Camera& otherCam)
{
	m_position = otherCam.m_position;
	m_rotation = otherCam.m_rotation;
	m_viewMatrix = otherCam.m_viewMatrix;
	m_projectionMatrix = otherCam.m_projectionMatrix;

	return *this;
}

const DirectX::XMMATRIX& Camera::GetViewMatrix() const { return m_viewMatrix; }
const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const { return m_projectionMatrix; }

void Camera::UpdateViewMatrix()
{
	const dx::XMVECTOR lookatVector =
		dx::XMVector3Transform(
			DEFAULT_FWD,
			dx::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, 0.0f)
		);

	const dx::XMVECTOR pos = dx::XMLoadFloat3(&m_position);
	const dx::XMVECTOR target = dx::XMVectorAdd(pos, lookatVector);
	m_viewMatrix = dx::XMMatrixLookAtLH(pos, target, DEFAULT_UP);
}
void Camera::UpdateProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRad = (fovDegrees / 360.0f) * dx::XM_2PI;
	m_projectionMatrix = dx::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
}



void Camera::AddPosition(float right, float up, float forward)
{
	// Move right, up, forward relative to the rotation of the camera
	dx::XMFLOAT3 translation = { right, up, forward };
	dx::XMStoreFloat3(
		&translation, 
		dx::XMVector3Transform(
			dx::XMLoadFloat3(&translation),
			dx::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, 0.0f) * 
			dx::XMMatrixScaling(TRAVEL_SPEED, TRAVEL_SPEED, TRAVEL_SPEED))
	);

	m_position = {
		m_position.x + translation.x,
		m_position.y + translation.y,
		m_position.z + translation.z
	};

}

void Camera::AddRotation(float pitch, float yaw, float roll)
{
	// pitch - clamp (no spinning)
	// yaw - wrap (spinning)
	// Roll currently unused
	
	//Clamp is a c++17 function from <algoritm>
	const float nonPerpendicularConst = 0.9995f;
	m_rotation.x = std::clamp( 
		m_rotation.x + pitch * ROTATION_SPEED, // pitch + add_pitch * speedlimit
		nonPerpendicularConst * -dx::XM_PI / 2.0f, // min
		nonPerpendicularConst * dx::XM_PI / 2.0f // max
	);

	// Yaw
	m_rotation.y += yaw * ROTATION_SPEED;

	// Roll

}