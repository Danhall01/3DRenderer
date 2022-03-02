#include "Camera.h"
namespace dx = DirectX;

Camera::Camera()
{
	m_FPosition = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_PositionVec = dx::XMLoadFloat3(&m_FPosition);

	m_FRotation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_RotationVec = dx::XMLoadFloat3(&m_FRotation);

	UpdateViewMatrix();
}
Camera::Camera(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	SetProjectionMatrix(fovDegrees, aspectRatio, nearZ, farZ);
	m_FRotation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_RotationVec = dx::XMLoadFloat3(&m_FRotation);
	SetPosition(pos);
}
Camera::Camera(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	SetProjectionMatrix(fovDegrees, aspectRatio, nearZ, farZ);
	m_FRotation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_RotationVec = dx::XMLoadFloat3(&m_FRotation);
	SetPosition(x, y, z);
}


Camera Camera::operator=(const Camera& otherCam)
{
	this->m_PositionVec = otherCam.m_PositionVec;
	this->m_RotationVec = otherCam.m_RotationVec;
	this->m_ViewMatrix  = otherCam.m_ViewMatrix;
	this->m_ProjectionMatrix = otherCam.m_ProjectionMatrix;
	this->m_FPosition = otherCam.m_FPosition;
	this->m_FRotation = otherCam.m_FRotation;
	return *this;
}


const DirectX::XMMATRIX& Camera::GetViewMatrix() const { return m_ViewMatrix; }
const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const { return m_ProjectionMatrix; }

const DirectX::XMVECTOR& Camera::GetPositionVector() const { return m_PositionVec; }
const DirectX::XMFLOAT3& Camera::GetPositionFloat3() const { return m_FPosition; }

const DirectX::XMVECTOR& Camera::GetRotationVector() const { return m_RotationVec; }
const DirectX::XMFLOAT3& Camera::GetRotationFloat3() const { return m_FRotation; }


void Camera::UpdateViewMatrix()
{
	//Create the rotationMatrix
	dx::XMMATRIX camRotationMatrix = dx::XMMatrixRotationRollPitchYaw(m_FRotation.x, m_FRotation.y, m_FRotation.z);

	//Transform the forward vector
	dx::XMVECTOR camLookAtPos = dx::XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	camLookAtPos = dx::XMVectorAdd(camLookAtPos, m_PositionVec);

	//Transform the up vector
	dx::XMVECTOR upDir = dx::XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);

	//Set the new ViewMatrix
	m_ViewMatrix = dx::XMMatrixLookAtLH(m_PositionVec, camLookAtPos, upDir);
}

void Camera::SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRad = (fovDegrees / 360.0f) * dx::XM_2PI;
	m_ProjectionMatrix = dx::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
}

void Camera::SetPosition(const DirectX::XMVECTOR& pos)
{
	dx::XMStoreFloat3(&m_FPosition, pos);
	m_PositionVec = pos;

	UpdateViewMatrix();
}
void Camera::AddPosition(const DirectX::XMVECTOR& addPos)
{
	m_PositionVec = dx::XMVectorAdd(m_PositionVec, addPos);
	dx::XMStoreFloat3(&m_FPosition, m_PositionVec);

	UpdateViewMatrix();
}
void Camera::SetPosition(float x, float y, float z)
{
	m_FPosition = dx::XMFLOAT3(x, y, z);
	m_PositionVec = dx::XMLoadFloat3(&m_FPosition);

	UpdateViewMatrix();
}
void Camera::AddPosition(float x, float y, float z)
{
	m_FPosition.x += x;
	m_FPosition.y += y;
	m_FPosition.z += z;
	m_PositionVec = dx::XMLoadFloat3(&m_FPosition);

	UpdateViewMatrix();
}


void Camera::SetRotation(const DirectX::XMVECTOR& rot)
{
	dx::XMStoreFloat3(&m_FRotation, rot);
	m_RotationVec = rot;

	UpdateViewMatrix();
}
void Camera::AddRotation(const DirectX::XMVECTOR& addRot)
{
	m_RotationVec = dx::XMVectorAdd(m_RotationVec, addRot);
	dx::XMStoreFloat3(&m_FPosition, m_PositionVec);

	UpdateViewMatrix();
}
void Camera::Setrotation(float x, float y, float z)
{
	m_FRotation = dx::XMFLOAT3(x, y, z);
	m_RotationVec = dx::XMLoadFloat3(&m_FRotation);

	UpdateViewMatrix();
}
void Camera::AddRotation(float x, float y, float z)
{
	m_FRotation.x += x;
	m_FRotation.y += y;
	m_FRotation.z += z;
	m_RotationVec = dx::XMLoadFloat3(&m_FRotation);

	UpdateViewMatrix();
}
