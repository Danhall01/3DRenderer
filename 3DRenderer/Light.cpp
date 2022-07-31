#include "Light.h"

Light::Light(){}
Light::~Light(){}

void Light::SetMatrix(const dx::XMMATRIX& matrix)
{
	m_matrix = matrix;
}
const dx::XMMATRIX& Light::GetMatrix() const
{
	return m_matrix;
}

void Light::SetColor(std::array<float, 3> clr)
{
	m_color = clr;
}
const std::array<float, 3>& Light::GetColor() const
{
	return m_color;
}

void Light::SetPosition(std::array<float, 3> pos)
{
	m_position = pos;
}
const std::array<float, 3>& Light::GetPosition() const
{
	return m_position;
}
