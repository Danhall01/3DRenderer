#pragma once
#include <d3d11.h>
#include <vector>

#include "Structures.h"
#include "wWindow.h"
#include "Camera.h"

#include <wrl/client.h>
using namespace Microsoft;

constexpr UINT MAX_CUBES = 1;
enum TEXTURE_CUBE_FACE_INDEX
{
	POSITIVE_X = 0,
	NEGATIVE_X = 1,
	POSITIVE_Y = 2,
	NEGATIVE_Y = 3,
	POSITIVE_Z = 4,
	NEGATIVE_Z = 5
};

class DCEMAsset
{
public: // Core Functions
	DCEMAsset();
	~DCEMAsset();

	bool Init(ID3D11Device* device, float x, float y, float z, int height, int width);


	ID3D11Texture2D* GetCubeMapP() const;
	ID3D11Texture2D*const* GetCubeMapPP() const;

	ID3D11ShaderResourceView* GetSRVP() const;
	ID3D11ShaderResourceView*const* GetSRVPP() const;

	ID3D11UnorderedAccessView* GetUAVP(TEXTURE_CUBE_FACE_INDEX faceIndex) const;
	ID3D11UnorderedAccessView* const* GetUAVPP(TEXTURE_CUBE_FACE_INDEX faceIndex) const;

	ID3D11DepthStencilView* GetDSVP() const;
	ID3D11DepthStencilView*const* GetDSVPP() const;

	Camera GetFaceIndexCamera(TEXTURE_CUBE_FACE_INDEX faceIndex);
	D3D11_VIEWPORT GetViewport() const;


private: // Helper Functions
	HRESULT BuildCubeMap(ID3D11Device* device, int height, int width);
	HRESULT BuildSRV(ID3D11Device* device);
	HRESULT BuildUAVList(ID3D11Device* device);
	HRESULT BuildCameras(float x, float y, float z, int height, int width);
	HRESULT BuildDSV(ID3D11Device* device, int height, int width);
	HRESULT BuildViewport(int height, int width);


private: // Misc Variables
	DXGI_FORMAT m_format;


private: // D3D11 Variables
	WRL::ComPtr<ID3D11Texture2D> m_cubeMap;
	WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	std::vector<WRL::ComPtr<ID3D11UnorderedAccessView>> m_uavList;

	std::vector<Camera> m_cameras;

	WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	D3D11_VIEWPORT m_viewport;
};

