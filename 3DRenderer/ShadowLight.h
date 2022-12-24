#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft;
#include "Camera.h"
#include <vector>
#include "Structures.h"
#include "wWindow.h"

struct CbufferData 
{
	dx::XMMATRIX VPMatrix;
};

// You can add lights but not remove them (adding creates a constant buffer)
// Once lights are added they will be statically placed, not movable
class ShadowLight
{
public:
	ShadowLight();
	ShadowLight(UINT maxLightCount);
	~ShadowLight();

	HRESULT Init(ID3D11Device* device, const wWindow& window);

	bool AddLight(const Light& light, const wWindow& window);

	const Light& GetLightData(int index) const;
	const std::vector<Light>& GetLightVec();


	dx::XMMATRIX GetLightVPMatrix(int index) const;
	dx::XMMATRIX At(int index) const;
	dx::XMMATRIX operator[](int index) const;

	const Camera& GetDXCamera(int index) const;
	
	const UINT Length() const;

	ID3D11DepthStencilView* GetDSVP(int index) const;
	ID3D11DepthStencilView*const* GetDSVPP(int index) const;

	ID3D11ShaderResourceView* GetSRVP() const;
	ID3D11ShaderResourceView*const* GetSRVPP() const;

	ID3D11SamplerState* GetShadowSamplerP() const;
	ID3D11SamplerState*const* GetShadowSamplerPP() const;

private:
	HRESULT InitShadowMap(ID3D11Device* device, const wWindow& window);
	HRESULT InitDSV(ID3D11Device* device);
	HRESULT InitSRV(ID3D11Device* device);

	HRESULT InitSampler(ID3D11Device* device);
public:
private:
	const UINT MAX_LIGHT_COUNT; // Default 10

	std::vector<Light> m_lightData;
	std::vector<Camera> m_lightCams;

	WRL::ComPtr<ID3D11SamplerState> m_shadowSampler;

	WRL::ComPtr<ID3D11Texture2D> m_shadowMap;
	WRL::ComPtr<ID3D11ShaderResourceView> m_sMapSRV;
	std::vector < WRL::ComPtr<ID3D11DepthStencilView>> m_sMapDSV;
};

