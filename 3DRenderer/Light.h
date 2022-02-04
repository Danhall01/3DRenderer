#pragma once
#include <vector>
#include <array>
#include <DirectXMath.h>

struct LightData
{
	float Position[3];
	float Color[3];
	float shininess;
};

class Light
{
public:
	Light(std::array<float,3> pos, std::array<float, 3> clr, float shininess, DirectX::XMMATRIX transform);
	~Light();

	const LightData& GetLightData() const;

	void UpdateTransformMatrix(DirectX::XMMATRIX transform);
	void ChangeColor(float clr[3]);
	void ChangeShininess(float shi);

private:
	//FRÅGA
	// Flyttar sig ljuset i worldspace eller i viewspace?
	//

private:
	float _Position[3];
	float _Color[3];
	float _Shininess;
	DirectX::XMMATRIX _Transform;

};

