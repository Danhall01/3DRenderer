#pragma once
#include <cstdint>
#include <d3d11.h>
#include <string>
#include <array>

class Texture
{
public:
	Texture();
	Texture(std::string id);
	~Texture();

	bool Empty() const;
	void Clear();

	void SetSpecularHighlight(float ns);
	void SetAmbientClr(std::array<float, 3> xyz);
	void SetDiffuseClr(std::array<float, 3> xyz);
	void SetSpecularClr(std::array<float, 3> xyz);
	void SetEmissiveCoef(std::array<float, 3> xyz);
	void SetOpticalDensity(float ni);
	void SetDissolve(float factor);

	void SetId(std::string id);
	std::string GetId();


private:

private:
	std::string m_id;

	float m_Ns;
	std::array<float, 3> m_Ka;
	std::array<float, 3> m_Kd;
	std::array<float, 3> m_Ks;
	std::array<float, 3> m_Ke;
	float m_Ni;
	float m_d;
};

