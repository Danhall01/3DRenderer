//#pragma once
//#include <DirectXMath.h>
//
//
//class Camera
//{
//public:
//	Camera();
//	~Camera() = default;
//
//	const DirectX::XMMATRIX& GetViewMatrix() const;
//	const DirectX::XMMATRIX& GetProjectionMatrix() const;
//
//	const DirectX::XMVECTOR& GetPositionVector() const;
//	const DirectX::XMVECTOR& GetRotationVector() const;
//
//	const DirectX::XMFLOAT3& GetPositionFloat3() const;
//	const DirectX::XMFLOAT3& GetRotationFloat3() const;
//
//	//View matrix is automatically calculated based on other inputs, hence no setter
//	void SetProjectionMatrix(float fovDegrees, float aspectRatio, float nearZ, float farZ);
//
//
//	void SetPosition(const DirectX::XMVECTOR& pos);
//	void AddPosition(const DirectX::XMVECTOR& addPos);
//
//	void SetPosition(float x, float y, float z);
//	void AddPosition(float x, float y, float z);
//
//
//	void SetRotation(const DirectX::XMVECTOR& rot);
//	void AddRotation(const DirectX::XMVECTOR& addRot);
//
//	void Setrotation(float x, float y, float z);
//	void AddRotation(float x, float y, float z);
//
//private:
//	void UpdateViewMatrix();
//
//private:
//	DirectX::XMVECTOR _PositionVec;
//	DirectX::XMVECTOR _RotationVec;
//	DirectX::XMMATRIX _ViewMatrix;
//	DirectX::XMMATRIX _ProjectionMatrix;
//
//	DirectX::XMFLOAT3 _FPosition;
//	DirectX::XMFLOAT3 _FRotation;
//
//
//	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
//	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//};