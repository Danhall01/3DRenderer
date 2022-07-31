#pragma once
#include "Structures.h"
#include <vector>
#include <string>
#include <array>

class Texture
{
public:
	Texture();
	~Texture();

	const TextureData& Data() const;
	void SetKa(const std::array<float, 3>& value);
	void SetKd(const std::array<float, 3>& value);
	void SetKs(const std::array<float, 3>& value);
	void SetNs(float value);

	const std::string GetImageKa() const;
	const std::string GetImageKd() const;
	const std::string GetImageKs() const;

	void SetImageKa(const std::string& imgID);
	void SetImageKd(const std::string& imgID);
	void SetImageKs(const std::string& imgID);

	void Clear();
	bool Empty() const;

private:
	std::string m_imageKa;
	std::string m_imageKd;
	std::string m_imageKs;

	TextureData m_rawData;
};

