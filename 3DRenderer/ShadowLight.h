#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft;
#include "Camera.h"
#include <vector>
#include "Structures.h"
#include "wWindow.h"


// You can add lights but not remove them (adding creates a constant buffer)
// Once lights are added they will be statically placed, not movable

class ShadowLight
{
public:
	ShadowLight();
	~ShadowLight();

	HRESULT Init(int limit, ID3D11Device* device, const wWindow& window);

	bool AddLight(Light light, ID3D11Device* device);

	const Light& GetLightData(int index) const;
	const std::vector<Light>& GetLightVec();


	const dx::XMMATRIX& GetLightVPMatrix(int index) const;
	const dx::XMMATRIX& At(int index) const;
	const dx::XMMATRIX& operator[](int index) const;





	ID3D11ShaderResourceView* GetShadowMapSRV() const;
	ID3D11ShaderResourceView* const* GetShadowMapSRVPP() const;

	ID3D11SamplerState* GetShadowSampler() const;
	ID3D11SamplerState* const* GetShadowSamplerPP() const;


	HRESULT UpdateCBuffer(int index);

private:
	HRESULT InitSampler(ID3D11Device* device);
	HRESULT InitNewCBuffer(ID3D11Device* device);


	HRESULT InitShadowMap(ID3D11Device* device, const wWindow& window);
	HRESULT InitDSV(ID3D11Device* device);
	HRESULT InitSRV(ID3D11Device* device);


	HRESULT InitStructuredBuffer(ID3D11Device* device, const wWindow& window);
	HRESULT InitSBufferSRV(ID3D11Device* device);

public:
private:
	std::vector<Light> m_lightData;
	std::vector<Camera> m_lightCams;
	std::vector<WRL::ComPtr<ID3D11Buffer>> m_cBuffer; // Holds the VP matrix and uploads it, probably should be an array


	WRL::ComPtr<ID3D11Texture2D> m_shadowMap;
	WRL::ComPtr<ID3D11ShaderResourceView> m_sMapSRV;
	std::vector < WRL::ComPtr<ID3D11DepthStencilView>> m_sMapDSV;
	


	WRL::ComPtr<ID3D11Buffer> m_structuredBuffer;
	WRL::ComPtr<ID3D11ShaderResourceView> m_sBufferSRV;
};

