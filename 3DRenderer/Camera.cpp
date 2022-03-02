#include "Camera.h"
namespace dx = DirectX;


const DirectX::XMMATRIX& Camera::GetViewMatrix() const { return _ViewMatrix; }
const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const { return _ProjectionMatrix; }

const DirectX::XMVECTOR& Camera::GetPositionVector() const { return _PositionVec; }
const DirectX::XMFLOAT3& Camera::GetPositionFloat3() const { return _FPosition; }

const DirectX::XMVECTOR& Camera::GetRotationVector() const { return _RotationVec; }
const DirectX::XMFLOAT3& Camera::GetRotationFloat3() const { return _FRotation; }


void Camera::UpdateViewMatrix()
{
	//Create the rotationMatrix
	dx::XMMATRIX camRotationMatrix = dx::XMMatrixRotationRollPitchYaw(_FRotation.x, _FRotation.y, _FRotation.z);

	//Transform the forward vector
	dx::XMVECTOR camLookAtPos = dx::XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	camLookAtPos = dx::XMVectorAdd(camLookAtPos, _PositionVec);

	//Transform the up vector
	dx::XMVECTOR upDir = dx::XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);

	//Set the new ViewMatrix
	_ViewMatrix = dx::XMMatrixLookAtLH(_PositionVec, camLookAtPos, upDir);
}
Camera::Camera()
{
	_FPosition = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	_PositionVec = dx::XMLoadFloat3(&_FPosition);

	_FRotation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	_RotationVec = dx::XMLoadFloat3(&_FRotation);

	UpdateViewMatrix();
}

Camera::Camera(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	SetProjectionMatrix(fovDegrees, aspectRatio, nearZ, farZ);
	SetPosition(pos);
}

Camera::Camera(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	SetProjectionMatrix(fovDegrees, aspectRatio, nearZ, farZ);
	SetPosition(x, y, z);
}

void Camera::SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRad = (fovDegrees / 360.0f) * dx::XM_2PI;
	_ProjectionMatrix = dx::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
}


void Camera::SetPosition(const DirectX::XMVECTOR& pos)
{
	dx::XMStoreFloat3(&_FPosition, pos);
	_PositionVec = pos;

	UpdateViewMatrix();
}
void Camera::AddPosition(const DirectX::XMVECTOR& addPos)
{
	_PositionVec = dx::XMVectorAdd(_PositionVec, addPos);
	dx::XMStoreFloat3(&_FPosition, _PositionVec);

	UpdateViewMatrix();
}
void Camera::SetPosition(float x, float y, float z)
{
	_FPosition = dx::XMFLOAT3(x, y, z);
	_PositionVec = dx::XMLoadFloat3(&_FPosition);

	UpdateViewMatrix();
}
void Camera::AddPosition(float x, float y, float z)
{
	_FPosition.x += x;
	_FPosition.y += y;
	_FPosition.z += z;
	_PositionVec = dx::XMLoadFloat3(&_FPosition);

	UpdateViewMatrix();
}


void Camera::SetRotation(const DirectX::XMVECTOR& rot)
{
	dx::XMStoreFloat3(&_FRotation, rot);
	_RotationVec = rot;

	UpdateViewMatrix();
}
void Camera::AddRotation(const DirectX::XMVECTOR& addRot)
{
	_RotationVec = dx::XMVectorAdd(_RotationVec, addRot);
	dx::XMStoreFloat3(&_FPosition, _PositionVec);

	UpdateViewMatrix();
}
void Camera::Setrotation(float x, float y, float z)
{
	_FRotation = dx::XMFLOAT3(x, y, z);
	_RotationVec = dx::XMLoadFloat3(&_FRotation);

	UpdateViewMatrix();
}
void Camera::AddRotation(float x, float y, float z)
{
	_FRotation.x += x;
	_FRotation.y += y;
	_FRotation.z += z;
	_RotationVec = dx::XMLoadFloat3(&_FRotation);

	UpdateViewMatrix();
}
