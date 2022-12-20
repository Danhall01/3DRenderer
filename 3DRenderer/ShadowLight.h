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
	~ShadowLight();

	HRESULT Init(ID3D11Device* device, const wWindow& window);

	bool AddLight(const Light& light, const wWindow& window);

	const Light& GetLightData(int index) const;
	const std::vector<Light>& GetLightVec();


	dx::XMMATRIX GetLightWVPMatrix(int index) const;
	dx::XMMATRIX At(int index) const;
	dx::XMMATRIX operator[](int index) const;



	ID3D11ShaderResourceView* GetShadowMapSRV() const;
	ID3D11ShaderResourceView* const* GetShadowMapSRVPP() const;


	bool UpdateCamConstantBuffer(int index, ID3D11DeviceContext* dContext);
private:
	HRESULT InitConstantBuffer(ID3D11Device* device);

	HRESULT InitShadowMap(ID3D11Device* device, const wWindow& window);
	HRESULT InitDSV(ID3D11Device* device);
	HRESULT InitSRV(ID3D11Device* device);


	HRESULT InitStructuredBuffer(ID3D11Device* device, const wWindow& window);
	HRESULT InitSBufferSRV(ID3D11Device* device, const wWindow& window);

public:
private:
	std::vector<Light> m_lightData;
	WRL::ComPtr<ID3D11Buffer> m_cCamBuffer;
	std::vector<Camera> m_lightCams;


	WRL::ComPtr<ID3D11Texture2D> m_shadowMap;
	WRL::ComPtr<ID3D11ShaderResourceView> m_sMapSRV;
	std::vector < WRL::ComPtr<ID3D11DepthStencilView>> m_sMapDSV;
	


	WRL::ComPtr<ID3D11Buffer> m_structuredBuffer;
	WRL::ComPtr<ID3D11ShaderResourceView> m_sBufferSRV;
};

