#include "ShadowLight.h"

constexpr int SHADOW_MAP_ARRAY_SIZE = 3;// spot, point, directional

ShadowLight::ShadowLight()
{
    // Set up the DXCams


}

ShadowLight::~ShadowLight()
{}

HRESULT ShadowLight::Init(int limit, ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};
    if (FAILED(InitShadowMap(device, window)))
        return hr;
    if (FAILED(InitDSV(device)))
        return hr;
    if (FAILED(InitSRV(device)))
        return hr;
    if (FAILED(InitStructuredBuffer(device, window)))
        return hr;
    if (FAILED(InitSBufferSRV(device)))
        return hr;

    return hr;
}

bool ShadowLight::AddLight(Light light, ID3D11Device* device)
{



    return false;
}



const Light& ShadowLight::GetLightData(int index) const
{
    return m_lightData.at(index);
}
const std::vector<Light>& ShadowLight::GetLightVec()
{
    return m_lightData;
}



const dx::XMMATRIX& ShadowLight::GetLightVPMatrix(int index) const
{
    return (*this)[index];
}
const dx::XMMATRIX& ShadowLight::At(int index) const
{
    return (*this)[index];
}
const dx::XMMATRIX& ShadowLight::operator[](int index) const
{
    if (index < 0 || index > m_lightData.size())
    {
        return dx::XMMatrixIdentity();
    }
    return m_lightCams[index].GetViewMatrix() *
        m_lightCams[index].GetProjectionMatrix();
}


ID3D11ShaderResourceView* ShadowLight::GetShadowMapSRV() const
{
    return m_sMapSRV.Get();
}
ID3D11ShaderResourceView* const* ShadowLight::GetShadowMapSRVPP() const
{
    return m_sMapSRV.GetAddressOf();
}


HRESULT ShadowLight::UpdateCBuffer(int index)
{
    if (index > m_cBuffer.size() || index < 0)
        return HRESULT(E_ACCESSDENIED);




    return E_NOTIMPL;
}
HRESULT ShadowLight::InitNewCBuffer(ID3D11Device* device)
{


    return E_NOTIMPL;
}


HRESULT ShadowLight::InitShadowMap(ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format             = DXGI_FORMAT_R32_TYPELESS;
    desc.ArraySize          = SHADOW_MAP_ARRAY_SIZE;
    desc.Height             = window.GetWindowHeight();
    desc.Width              = window.GetWindowWidth();
    desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage              = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags     = 0;
    desc.MipLevels          = 1;
    desc.MiscFlags          = 0;


    hr = device->CreateTexture2D(
        &desc, 
        nullptr, 
        m_shadowMap.GetAddressOf()
    );
    return hr;
}
HRESULT ShadowLight::InitDSV(ID3D11Device* device)
{
    HRESULT hr = {};

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.MipSlice = 0;
    dsvDesc.Texture2DArray.ArraySize = 1;

    for (UINT i = 0; i < SHADOW_MAP_ARRAY_SIZE; ++i)
    {
        ID3D11DepthStencilView* dsv = {};

        dsvDesc.Texture2DArray.FirstArraySlice = i;
        hr = device->CreateDepthStencilView(
            m_shadowMap.Get(),
            &dsvDesc,
            &dsv
        );

        m_sMapDSV.push_back(dsv);

        if(FAILED(hr))
        {
            break;
        }
    }

    return hr;
}
HRESULT ShadowLight::InitSRV(ID3D11Device* device)
{
    HRESULT hr = {};

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                         = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels       = 1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize       = SHADOW_MAP_ARRAY_SIZE;

    hr = device->CreateShaderResourceView(
        m_shadowMap.Get(),
        &srvDesc,
        m_sMapSRV.GetAddressOf()
    );
    return hr;
}

HRESULT ShadowLight::InitStructuredBuffer(ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth           = sizeof(float) * window.GetWindowHeight() * window.GetWindowWidth(); // Maybe wrong?
    desc.Usage               = D3D11_USAGE_DEFAULT;
    desc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags      = 0; // Not dynamic
    desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = sizeof(float);

    hr = device->CreateBuffer(
        &desc,
        nullptr,
        m_structuredBuffer.GetAddressOf()
    );
    return hr;
}
HRESULT ShadowLight::InitSBufferSRV(ID3D11Device* device)
{
    HRESULT hr = {};

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format;
    srvDesc.ViewDimension;
    srvDesc.Buffer.FirstElement;
    srvDesc.Buffer.NumElements;

    hr = device->CreateShaderResourceView(
        m_structuredBuffer.Get(),
        &srvDesc,
        m_sBufferSRV.GetAddressOf()
    );

    return hr;
}
