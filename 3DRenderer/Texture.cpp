#include "Texture.h"
Texture::Texture()
{
	m_Ns = 0;
	m_Ka = {};
	m_Kd = {};
	m_Ks = {};
	m_Ke = {};
	m_Ni = 0;
	m_d = 0;
}
Texture::Texture(std::string id)
{
	m_id = id;
	m_Ns = 0;
	m_Ka = {};
	m_Kd = {};
	m_Ks = {};
	m_Ke = {};
	m_Ni = 0;
	m_d = 0;
}
Texture::~Texture()
{}

bool Texture::Empty() const
{
	return m_id.empty();
}
void Texture::Clear()
{
	m_id.clear();
}



void Texture::SetSpecularHighlight(float ns) { m_Ns = ns; }
void Texture::SetAmbientClr(std::array<float, 3> xyz) { m_Ka = xyz; }
void Texture::SetDiffuseClr(std::array<float, 3> xyz) { m_Kd = xyz; }
void Texture::SetSpecularClr(std::array<float, 3> xyz) { m_Ks = xyz; }
void Texture::SetEmissiveCoef(std::array<float, 3> xyz) { m_Ke = xyz; }
void Texture::SetOpticalDensity(float ni) { m_Ni = ni; }
void Texture::SetDissolve(float factor) { m_d = factor; }
void Texture::SetId(std::string id) { m_id = id; }

std::string Texture::GetId() { return m_id; }
