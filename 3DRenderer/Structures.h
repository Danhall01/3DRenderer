#pragma once
#include <array>

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
struct WVPMatrix
{
	dx::XMMATRIX WorldMatrix;
	dx::XMMATRIX ViewMatrix;
	dx::XMMATRIX ProjectionMatrix;
};
struct shaderSet
{
	//Shaders
	WRL::ComPtr<ID3D11VertexShader>     vertexShader;
	WRL::ComPtr<ID3D11HullShader>       hullShader;
	WRL::ComPtr<ID3D11DomainShader>     domainShader;
	WRL::ComPtr<ID3D11GeometryShader>   geometryShader;
	WRL::ComPtr<ID3D11PixelShader>      pixelShader;

	//WRL::ComPtr<ID3D11ComputeShader>  m_computeShader;
};