#pragma once
#include <cstdint>
#include <d3d11.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum flag
{
	test = 0,		// 0000
	test2 = 1,		// 0001
	test3 = 1 << 1, // 0010
	test4 = 1 << 2, // 0100
	test5 = 1 << 3, // 1000
};


class Texture
{
public:
	Texture(std::string filepath, flag flags);
	~Texture();

	const uint8_t*& GetImage();
	const ID3D11ShaderResourceView*& GetSRV();
	const ID3D11SamplerState*& GetSamplerState();

	void SetImage(std::string filepath);


private:
	void SetupSRV(flag flags);
	void SetupSamplerState(flag flags);

private:
	uint8_t* img;
	ID3D11ShaderResourceView* _SRV;
	ID3D11SamplerState* _SampleState;
};

