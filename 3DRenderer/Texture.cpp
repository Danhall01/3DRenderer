#include "Texture.h"

Texture::Texture()
{
	m_rawData = {};
}
Texture::~Texture()
{}

const TextureData& Texture::Data() const
{
	return m_rawData;
}

void Texture::SetKa(const std::array<float, 3>& value)
{
	m_rawData.Ka = value;
}
void Texture::SetKd(const std::array<float, 3>& value)
{
	m_rawData.Kd = value;
}
void Texture::SetKs(const std::array<float, 3>& value)
{
	m_rawData.Ks = value;
}
void Texture::SetNs(float value)
{
	m_rawData.Ns = value;
}

const std::string Texture::GetImageKa() const
{
	return m_imageKa;
}
const std::string Texture::GetImageKd() const
{
	return m_imageKd;
}
const std::string Texture::GetImageKs() const
{
	return m_imageKs;
}

void Texture::SetImageKa(const std::string& imgID)
{
	m_imageKa = imgID;
}
void Texture::SetImageKd(const std::string & imgID)
{
	m_imageKd = imgID;
}
void Texture::SetImageKs(const std::string & imgID)
{
	m_imageKs = imgID;
}

void Texture::Clear()
{
	m_imageKa.clear();
	m_imageKd.clear();
	m_imageKs.clear();
	m_rawData.Clear();
}
bool Texture::Empty() const
{
	return m_rawData.Empty();
}
