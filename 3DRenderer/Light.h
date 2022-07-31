#pragma once
#include <DirectXMath.h>
#include "Structures.h"
#include <array>

class Light
{
public:
	Light();
	~Light();


	void SetMatrix(const dx::XMMATRIX& matrix);
	const dx::XMMATRIX& GetMatrix() const;

	void SetColor(std::array<float, 3> clr);
	const std::array<float, 3>& GetColor() const;

	void SetPosition(std::array<float, 3> pos);
	const std::array<float, 3>& GetPosition() const;


private:
	

private:
	std::array<float, 3> m_position;
	std::array<float, 3> m_color;
	dx::XMMATRIX m_matrix;

};

