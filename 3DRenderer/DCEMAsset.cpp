#include "DCEMAsset.h"

DCEMAsset::DCEMAsset()
{ 
	m_viewport = {0}; 
	m_format = DXGI_FORMAT_UNKNOWN;
}
DCEMAsset::~DCEMAsset()
{}

bool DCEMAsset::Init(ID3D11Device* device, float x, float y, float z, int height, int width)
{
	m_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	

	if ( !SUCCEEDED(BuildCubeMap(device, height, width)) )  { return false; };
	if ( !SUCCEEDED(BuildUAVList(device)) )                 { return false; };
	if ( !SUCCEEDED(BuildSRV(device)) )                     { return false; };
	if ( !SUCCEEDED(BuildCameras(x, y, z, height, width)) ) { return false; };
	if ( !SUCCEEDED(BuildDSV(device, height, width)) )		{ return false; };
	if ( !SUCCEEDED(BuildViewport(height, width)) )         { return false; };
	return SUCCEEDED(S_OK);
}



HRESULT DCEMAsset::BuildCubeMap(ID3D11Device* device, int height, int width)
{
	HRESULT hr = {};
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format             = m_format;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.Height             = height;
	desc.Width              = width;
	desc.ArraySize          = 6 * MAX_CUBES;
	desc.MipLevels          = 1;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags          = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(
		&desc,
		nullptr,
		m_cubeMap.GetAddressOf()
	);
	return hr;
}
HRESULT DCEMAsset::BuildSRV(ID3D11Device* device)
{
	HRESULT hr = device->CreateShaderResourceView(
		m_cubeMap.Get(),
		nullptr, // NULL allows to access the entire resource
		m_srv.GetAddressOf()
	);

	return hr;
}
HRESULT DCEMAsset::BuildUAVList(ID3D11Device* device)
{
	HRESULT hr = {};
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Buffer;
	desc.Format = m_format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = 1;
	desc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		ID3D11UnorderedAccessView* uav;
		desc.Texture2DArray.FirstArraySlice = i;

		hr = device->CreateUnorderedAccessView(
			m_cubeMap.Get(),
			&desc,
			&uav
		);
		if (FAILED(hr))
		{
			return hr;
		}
		m_uavList.push_back(uav);
		uav->Release();
	}
	return hr;
}
HRESULT DCEMAsset::BuildCameras(float x, float y, float z, int height, int width)
{
	struct eulerAngles
	{
		float pitch;
		float yaw;
		float roll;
	};
	eulerAngles rotations[6] = {0};

	rotations[POSITIVE_Y] = {  dx::XM_PIDIV2, 0.0f, 0.0f };
	rotations[NEGATIVE_Y] = { -dx::XM_PIDIV2, 0.0f, 0.0f };

	rotations[POSITIVE_X] = { 0.0f, -dx::XM_PIDIV2, 0.0f };
	rotations[NEGATIVE_X] = { 0.0f,  dx::XM_PIDIV2, 0.0f };

	rotations[POSITIVE_Z] = { dx::XM_PI, 0.0f, dx::XM_PIDIV2 };
	rotations[NEGATIVE_Z] = {      0.0f, 0.0f, dx::XM_PIDIV2 };



	for (int i = 0; i < 6; i++) 
	{
		m_cameras.push_back(
			Camera( x, y, z,
					rotations[i].pitch, rotations[i].yaw, rotations[i].roll,
					90, 1.0f, 0.1f, 100.0f));	
	}
	
	return S_OK;
}
HRESULT DCEMAsset::BuildDSV(ID3D11Device* device, int height, int width)
{
	HRESULT hr = {};

	ID3D11Texture2D* dsvTexture = {};
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format             = DXGI_FORMAT_D32_FLOAT;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.Height             = height;
	desc.Width              = width;
	desc.ArraySize          = 1;
	desc.MipLevels          = 1;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;
	hr = device->CreateTexture2D(&desc, nullptr, &dsvTexture);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format                   = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension            = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.MipSlice  = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;

	hr = device->CreateDepthStencilView(
		dsvTexture,
		&dsvDesc,
		m_dsv.GetAddressOf()
	);
	dsvTexture->Release();

	return hr;
}
HRESULT DCEMAsset::BuildViewport(int height, int width)
{
	m_viewport.Height   = static_cast<FLOAT>(height);
	m_viewport.Width    = static_cast<FLOAT>(width);
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;
	m_viewport.TopLeftX = 0; // o --
	m_viewport.TopLeftY = 0; // |
	return S_OK;
}




ID3D11Texture2D* DCEMAsset::GetCubeMapP() const
{
	return m_cubeMap.Get();
}
ID3D11Texture2D* const* DCEMAsset::GetCubeMapPP() const
{
	return m_cubeMap.GetAddressOf();
}

ID3D11ShaderResourceView* DCEMAsset::GetSRVP() const
{
	return m_srv.Get();
}
ID3D11ShaderResourceView* const* DCEMAsset::GetSRVPP() const
{
	return m_srv.GetAddressOf();
}

ID3D11UnorderedAccessView* DCEMAsset::GetUAVP(TEXTURE_CUBE_FACE_INDEX faceIndex) const
{
	return m_uavList[faceIndex].Get();
}
ID3D11UnorderedAccessView* const* DCEMAsset::GetUAVPP(TEXTURE_CUBE_FACE_INDEX faceIndex) const
{
	return m_uavList[faceIndex].GetAddressOf();
}

ID3D11DepthStencilView* DCEMAsset::GetDSVP() const
{
	return m_dsv.Get();
}
ID3D11DepthStencilView* const* DCEMAsset::GetDSVPP() const
{
	return m_dsv.GetAddressOf();
}

Camera DCEMAsset::GetFaceIndexCamera(TEXTURE_CUBE_FACE_INDEX faceIndex)
{
	return m_cameras[faceIndex];
}

D3D11_VIEWPORT DCEMAsset::GetViewport() const
{
	return m_viewport;
}