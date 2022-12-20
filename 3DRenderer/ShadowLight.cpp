#include "ShadowLight.h"

constexpr int SHADOW_MAP_ARRAY_SIZE = 3;// spot, point, directional


ShadowLight::ShadowLight() 
{}

ShadowLight::~ShadowLight() 
{}


bool ShadowLight::AddLight(const Light& light, const wWindow& window)
{
    float fovDeg = 90;
    float aspectRatio = window.GetWindowRatio(); //hard coded from base values

    Camera dxCam = Camera(
        light.Position_Type[0], light.Position_Type[1], light.Position_Type[2],
        light.Direction_Range[0], light.Direction_Range[1], light.Direction_Range[2],
        fovDeg,
        aspectRatio,
        0.1f, light.Direction_Range[3]
    );

    m_lightCams.push_back(dxCam);
    m_lightData.push_back(light);
    return true;

    //if(light.Position_Type[3] == lightType::LIGHT_TYPE_SPOTLIGHT)
    //{
    //    Camera dxCam = Camera(
    //        light.Position_Type[0], light.Position_Type[1], light.Position_Type[2],
    //        light.Direction_Range[0], light.Direction_Range[1], light.Direction_Range[2],
    //        fovDeg,
    //        aspectRatio,
    //        0.1f, light.Direction_Range[3]
    //    );
    //    m_lightCams.push_back(dxCam);
    //}
    //else if (light.Position_Type[3] == lightType::LIGHT_TYPE_DIRECTIONAL)
    //{
    //    Camera dxCam = Camera(
    //        light.Position_Type[0], light.Position_Type[1], light.Position_Type[2],
    //        light.Direction_Range[0], light.Direction_Range[1], light.Direction_Range[2],
    //        fovDeg,
    //        aspectRatio,
    //        0.1f, light.Direction_Range[3]
    //    );
    //    m_lightCams.push_back(dxCam);
    //}
    //else if (light.Position_Type[3] == lightType::LIGHT_TYPE_POINT)
    //{
    //    Camera dxCam = Camera(
    //        light.Position_Type[0], light.Position_Type[1], light.Position_Type[2],
    //        light.Direction_Range[0], light.Direction_Range[1], light.Direction_Range[2],
    //        fovDeg,
    //        aspectRatio,
    //        0.1f, light.Direction_Range[3]
    //    );
    //    m_lightCams.push_back(dxCam);
    //}
    //else 
    //{
    //    return false;
    //}
}


const Light& ShadowLight::GetLightData(int index) const
{
    return m_lightData.at(index);
}
const std::vector<Light>& ShadowLight::GetLightVec()
{
    return m_lightData;
}


dx::XMMATRIX ShadowLight::GetLightWVPMatrix(int index) const
{
    return (*this)[index];
}
dx::XMMATRIX ShadowLight::At(int index) const
{
    return (*this)[index];
}
//Returns the WVP matrix for the given light (if any)
dx::XMMATRIX ShadowLight::operator[](int index) const
{
    if (index < 0 || index > m_lightData.size())
    {
        return dx::XMMatrixIdentity();
    }
    std::array<float, 4> pos = m_lightData[index].Position_Type;
    std::array<float, 4> rot = m_lightData[index].Direction_Range;
    
    return 
        dx::XMMatrixTranspose(dx::XMMatrixRotationRollPitchYaw(rot[0], rot[1], rot[2]) * dx::XMMatrixTranslation(pos[0], pos[1], pos[2])) *
        dx::XMMatrixTranspose(m_lightCams[index].GetViewMatrix()) *
        dx::XMMatrixTranspose(m_lightCams[index].GetProjectionMatrix());
}


ID3D11ShaderResourceView* ShadowLight::GetShadowMapSRV() const
{
    return m_sMapSRV.Get();
}
ID3D11ShaderResourceView* const* ShadowLight::GetShadowMapSRVPP() const
{
    return m_sMapSRV.GetAddressOf();
}



HRESULT ShadowLight::Init(ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};
    if (FAILED(hr = InitConstantBuffer(device)))
        return hr;

    if (FAILED(hr = InitShadowMap(device, window)))
        return hr;
    if (FAILED(hr = InitDSV(device)))
        return hr;
    if (FAILED(hr = InitSRV(device)))
        return hr;
    if (FAILED(hr = InitStructuredBuffer(device, window)))
        return hr;
    if (FAILED(hr = InitSBufferSRV(device, window)))
        return hr;

    return hr;
}

HRESULT ShadowLight::InitConstantBuffer(ID3D11Device* device)
{
    HRESULT hr = {};

    D3D11_BUFFER_DESC desc = {};
    desc.Usage               = D3D11_USAGE_DYNAMIC;
    desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    desc.ByteWidth           = sizeof(CbufferData);
    desc.StructureByteStride = 0;
    desc.MiscFlags           = 0;

    hr = device->CreateBuffer(
        &desc,
        nullptr,
        m_cCamBuffer.GetAddressOf()
    );
    return hr;
}
bool ShadowLight::UpdateCamConstantBuffer(int index, ID3D11DeviceContext* dContext)
{
    HRESULT hr = {};

    if (index > m_lightData.size() || index < 0)
    {
        hr = E_ACCESSDENIED;
        return false;
    }
    CbufferData data = { m_lightCams[index].GetViewMatrix() * m_lightCams[index].GetProjectionMatrix() };

    D3D11_MAPPED_SUBRESOURCE mResource = {};
    hr = dContext->Map(m_cCamBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
    if (FAILED(hr))
        return false;
    memcpy(
        mResource.pData,
        &data,
        sizeof(data)
    );
    dContext->Unmap(m_cCamBuffer.Get(), 0);


    return SUCCEEDED(hr);
}

HRESULT ShadowLight::InitShadowMap(ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format             = DXGI_FORMAT_R32_TYPELESS;
    desc.ArraySize          = SHADOW_MAP_ARRAY_SIZE;
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
    desc.ByteWidth           = static_cast<UINT>(sizeof(float) * window.GetWindowHeight() * window.GetWindowWidth()); // Maybe wrong?
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
HRESULT ShadowLight::InitSBufferSRV(ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements  = static_cast<UINT>(window.GetWindowHeight() * window.GetWindowWidth());

    hr = device->CreateShaderResourceView(
        m_structuredBuffer.Get(),
        &srvDesc,
        m_sBufferSRV.GetAddressOf()
    );

    return hr;
}
