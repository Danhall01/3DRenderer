#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft;
#include "Camera.h"
#include <vector>
#include "Structures.h"
#include "wWindow.h"

class ShadowLight
{
public:
	ShadowLight();
	~ShadowLight();

	HRESULT Init(int limit, ID3D11Device* device, const wWindow& window);

	bool AddLight(Light light);
	void MoveLight(int index, int right, int up, int forward);
	void RotateLight(int index, int pitch, int yaw, int roll);

	const Light& GetLightData(int index) const;
	const std::vector<Light>& GetLightVec();


	const dx::XMMATRIX& GetLightVPMatrix(int index) const;
	const dx::XMMATRIX& At(int index) const;
	const dx::XMMATRIX& operator[](int index) const;





	ID3D11ShaderResourceView* GetShadowMapSRV() const;
	ID3D11ShaderResourceView* const* GetShadowMapSRVPP() const;

	ID3D11RenderTargetView* GetShadowMapRTV() const;
	ID3D11RenderTargetView* const* GetShadowMapRTVPP() const;

	ID3D11SamplerState* GetShadowSampler() const;
	ID3D11SamplerState* const* GetShadowSamplerPP() const;


	HRESULT UpdateCBuffer(int index);

private:
	HRESULT InitTexture();
	HRESULT InitSampler();
	HRESULT InitDSV();
	HRESULT InitCBuffer();

public:
private:
	WRL::ComPtr<ID3D11SamplerState> m_sState;

	std::vector<Light> m_lightData;
	std::vector<Camera> m_lightCams;



	WRL::ComPtr<ID3D11ShaderResourceView> m_textureSRV;
	WRL::ComPtr<ID3D11RenderTargetView> m_textureRTV;
	WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	WRL::ComPtr<ID3D11Buffer> m_cbuffer;
};

