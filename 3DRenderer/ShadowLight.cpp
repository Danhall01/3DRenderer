#include "ShadowLight.h"


ShadowLight::ShadowLight() : MAX_LIGHT_COUNT(10)
{}
ShadowLight::ShadowLight(UINT maxLightCount) : MAX_LIGHT_COUNT(maxLightCount)
{}

ShadowLight::~ShadowLight() 
{}


bool ShadowLight::AddLight(const Light& light, const wWindow& window)
{
    if (m_lightData.size() >= MAX_LIGHT_COUNT)
        return false;

    // TODO: Make them change depending on type of light
    //float fovDeg = 90;
    //switch(static_cast<int>(light.Position_Type[3]))
    //{
    //case LIGHT_TYPE_SPOTLIGHT: fovDeg = 45;
    //}

    float fovDeg = 150;


    float aspectRatio = window.GetWindowRatio();

    Camera dxCam = Camera(
        light.Position_Type[0], light.Position_Type[1], light.Position_Type[2],
        light.Direction_Range[0], light.Direction_Range[1], light.Direction_Range[2],
        fovDeg,
        aspectRatio,
        0.1f, light.Direction_Range[3] + 50,
        static_cast<lightType>(light.Position_Type[3])
    );
    m_lightCams.push_back(dxCam);
    m_lightData.push_back(light);
    return true;
}


const Light& ShadowLight::GetLightData(int index) const
{
    return m_lightData.at(index);
}
const std::vector<Light>& ShadowLight::GetLightVec()
{
    return m_lightData;
}


dx::XMMATRIX ShadowLight::GetLightVPMatrix(int index) const
{
    return (*this)[index];
}
dx::XMMATRIX ShadowLight::At(int index) const
{
    return (*this)[index];
}
//Returns the VP matrix for the given light (if any)
dx::XMMATRIX ShadowLight::operator[](int index) const
{
    if (index < 0 || index > m_lightData.size())
    {
        return dx::XMMatrixIdentity();
    }    
    return (
        dx::XMMatrixTranspose(m_lightCams[index].GetProjectionMatrix()) *
        dx::XMMatrixTranspose(m_lightCams[index].GetViewMatrix())
        );
}

const Camera& ShadowLight::GetDXCamera(int index) const
{
    return m_lightCams[index];
}


const UINT ShadowLight::Length() const
{
    return static_cast<UINT>(m_lightData.size());
}


ID3D11DepthStencilView* ShadowLight::GetDSVP(int index) const
{
    return m_sMapDSV[index].Get();
}
ID3D11DepthStencilView*const* ShadowLight::GetDSVPP(int index) const
{
    return m_sMapDSV[index].GetAddressOf();
}


ID3D11ShaderResourceView* ShadowLight::GetSRVP() const
{
    return m_sMapSRV.Get();
}
ID3D11ShaderResourceView* const* ShadowLight::GetSRVPP() const
{
    return m_sMapSRV.GetAddressOf();
}

ID3D11SamplerState* ShadowLight::GetShadowSamplerP() const
{
    return m_shadowSampler.Get();
}
ID3D11SamplerState*const* ShadowLight::GetShadowSamplerPP() const
{
    return m_shadowSampler.GetAddressOf();
}




HRESULT ShadowLight::Init(ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};

    if (FAILED(hr = InitShadowMap(device, window)))
        return hr;
    if (FAILED(hr = InitDSV(device)))
        return hr;
    if (FAILED(hr = InitSRV(device)))
        return hr;
    if (FAILED(hr = InitSampler(device)))
        return hr;

    return hr;
}


HRESULT ShadowLight::InitShadowMap(ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format             = DXGI_FORMAT_R32_TYPELESS;
    desc.ArraySize          = MAX_LIGHT_COUNT;
    desc.Height             = static_cast<UINT>(window.GetWindowHeight());
    desc.Width              = static_cast<UINT>(window.GetWindowWidth());
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

    for (UINT i = 0; i < MAX_LIGHT_COUNT; ++i)
    {
        ID3D11DepthStencilView* dsv = {};

        dsvDesc.Texture2DArray.FirstArraySlice = i;
        hr = device->CreateDepthStencilView(
            m_shadowMap.Get(),
            &dsvDesc,
            &dsv
        );

        m_sMapDSV.push_back(dsv);
        dsv->Release();
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
    srvDesc.Texture2DArray.ArraySize       = MAX_LIGHT_COUNT;

    hr = device->CreateShaderResourceView(
        m_shadowMap.Get(),
        &srvDesc,
        m_sMapSRV.GetAddressOf()
    );
    return hr;
}

HRESULT ShadowLight::InitSampler(ID3D11Device* device)
{
    HRESULT hr = {};

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;


    hr = device->CreateSamplerState(
        &samplerDesc,
        m_shadowSampler.GetAddressOf()
    );

    return hr;
}
