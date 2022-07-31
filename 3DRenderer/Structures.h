#pragma once
#include <array>
#include <string>
#include <wrl/client.h>
using namespace Microsoft;
#include <d3d11.h>
#include <DirectXMath.h>
namespace dx = DirectX;

struct Vertex
{
	std::array<float, 3> position;
	std::array<float, 3> normal;
	std::array<float, 2> uv;
};
struct TextureData
{
	float Ns;
	std::array<float, 3> Ka;
	std::array<float, 3> Kd;
	std::array<float, 3> Ks;
	float padding[2];

	bool Empty() const
	{
		return (Ns == 0 &&
			Ka[0] == 0 &&
			Ka[1] == 0 &&
			Ka[2] == 0 &&

			Kd[0] == 0 &&
			Kd[1] == 0 &&
			Kd[2] == 0 &&

			Ks[0] == 0 &&
			Ks[1] == 0 &&
			Ks[2] == 0
			);
	}
	void Clear()
	{
		Ns = 0;
		Ka.fill(0);
		Kd.fill(0);
		Ks.fill(0);
	}
};
struct Image
{
	unsigned char* img;
	int width;
	int height;
	int channels;
};
struct Submesh
{
	std::string textureId;
	//Data
	unsigned int indiceCount;
	unsigned int indiceStartIndex;
	unsigned int verticeStartIndex;
	void Clear()
	{
		textureId.clear();
		indiceCount = 0;
		indiceStartIndex = 0;
		verticeStartIndex = 0;
	}
	bool Empty() const
	{
		return indiceCount == 0;
	}
};
struct WVPMatrix
{
	dx::XMMATRIX WorldMatrix;
	dx::XMMATRIX ViewMatrix;
	dx::XMMATRIX ProjectionMatrix;
};
struct ShaderSet
{
	//Shaders
	WRL::ComPtr<ID3D11VertexShader>     vertexShader;
	WRL::ComPtr<ID3D11HullShader>       hullShader;
	WRL::ComPtr<ID3D11DomainShader>     domainShader;
	WRL::ComPtr<ID3D11GeometryShader>   geometryShader;
	WRL::ComPtr<ID3D11PixelShader>      pixelShader;

	WRL::ComPtr<ID3D11ComputeShader>    computeShader;
};
struct GraphicsBuffer
{
	WRL::ComPtr<ID3D11Texture2D> texture;
	WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};
