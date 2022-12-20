#include "ShadowLight.h"
ShadowLight::ShadowLight()
{}

ShadowLight::~ShadowLight()
{}

HRESULT ShadowLight::Init(int limit, ID3D11Device* device, const wWindow& window)
{
    HRESULT hr = {};
    hr = InitTexture();
    if (FAILED(hr))
        return hr;

    hr = InitCBuffer();
    if (FAILED(hr))
        return hr;

    hr = InitDSV();
    if (FAILED(hr))
        return hr;

    hr = InitSampler();
    return hr;
}

bool ShadowLight::AddLight(Light light)
{
    return false;
}
void ShadowLight::MoveLight(int index, int right, int up, int forward)
{
    // TODO
}
void ShadowLight::RotateLight(int index, int pitch, int yaw, int roll)
{
    // TODO
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
    return nullptr;
}
ID3D11ShaderResourceView* const* ShadowLight::GetShadowMapSRVPP() const
{
    return nullptr;
}

ID3D11RenderTargetView* ShadowLight::GetShadowMapRTV() const
{
    return m_textureRTV.Get();
}
ID3D11RenderTargetView* const* ShadowLight::GetShadowMapRTVPP() const
{
    return m_textureRTV.GetAddressOf();
}

ID3D11SamplerState* ShadowLight::GetShadowSampler() const
{
    return m_sState.Get();
}
ID3D11SamplerState* const* ShadowLight::GetShadowSamplerPP() const
{
    return m_sState.GetAddressOf();
}



HRESULT ShadowLight::InitTexture()
{
    return E_NOTIMPL;
}
HRESULT ShadowLight::InitSampler()
{
    return E_NOTIMPL;
}