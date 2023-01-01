#include "Particle.h"
#include <d3dcompiler.h>



Particle::Particle() : m_particles(0)
{}
Particle::~Particle()
{}


HRESULT Particle::Init(ID3D11Device* device, std::vector<ParticleData>& data, UINT particleCount)
{
	HRESULT hr = {};

	hr = CompileShaders(device);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = BuildBuffer(device, data, particleCount);
	if (FAILED(hr))
	{
		return hr;
	}
	
	hr = BuildSRV(device, particleCount);
	if (FAILED(hr))
	{
		return hr;
	}
	
	hr = BuildUAV(device, particleCount);
	m_initialized = true;
	m_particles = particleCount;
	return hr;
}


HRESULT Particle::BuildBuffer(ID3D11Device* device, std::vector<ParticleData>& data, UINT particleCount)
{
	// Can't bind UAV when dynamic and CPU read is set and wise versa
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage               = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags      = 0;
	desc.ByteWidth           = sizeof(ParticleData) * particleCount;
	desc.StructureByteStride = sizeof(ParticleData);
	desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;


	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem          = data.data();
	subData.SysMemPitch      = 0;
	subData.SysMemSlicePitch = 0;

	return device->CreateBuffer(
		&desc,
		&subData,
		m_buffer.GetAddressOf()
	);
}
HRESULT Particle::BuildSRV(ID3D11Device* device, UINT particleCount)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format              = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension       = D3D_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements  = particleCount;

	return device->CreateShaderResourceView(
		m_buffer.Get(),
		&desc,
		m_srv.GetAddressOf()
	);
}
HRESULT Particle::BuildUAV(ID3D11Device* device, UINT particleCount)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format              = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements  = particleCount;
	desc.Buffer.Flags        = 0;

	return device->CreateUnorderedAccessView(
		m_buffer.Get(),
		&desc,
		m_uav.GetAddressOf()
	);
}

HRESULT Particle::CompileShaders(ID3D11Device* device)
{
	ID3DBlob* shaderBlob = {};
	HRESULT hr = {};
#if _DEBUG
	wchar_t vsPath[] = L"../x64/Debug/ParticleVS.cso";
	wchar_t gsPath[] = L"../x64/Debug/ParticleGS.cso";
	wchar_t psPath[] = L"../x64/Debug/ParticlePS.cso";
	wchar_t csPath[] = L"../x64/Debug/ParticleCS.cso";
#else
	wchar_t vsPath[] = L"../x64/Release/ParticleVS.cso";
	wchar_t gsPath[] = L"../x64/Release/ParticleGS.cso";
	wchar_t psPath[] = L"../x64/Release/ParticlePS.cso";
	wchar_t csPath[] = L"../x64/Release/ParticleCS.cso";
#endif

	// Vertex shader
	hr = D3DReadFileToBlob(vsPath, &shaderBlob);
	if (FAILED(hr))
		return hr;
	hr = device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL,
		m_vShader.GetAddressOf()
	);
	if (FAILED(hr))
		return hr;

	// Geometry shader
	hr = D3DReadFileToBlob(gsPath, &shaderBlob);
	if (FAILED(hr))
		return hr;
	hr = device->CreateGeometryShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL,
		m_gShader.GetAddressOf()
	);
	if (FAILED(hr))
		return hr;

	// Pixel shader
	hr = D3DReadFileToBlob(psPath, &shaderBlob);
	if (FAILED(hr))
		return hr;
	hr = device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL,
		m_pShader.GetAddressOf()
	);
	if (FAILED(hr))
		return hr;

	// Compute shader
	hr = D3DReadFileToBlob(csPath, &shaderBlob);
	if (FAILED(hr))
		return hr;
	hr = device->CreateComputeShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL,
		m_cShader.GetAddressOf()
	);

	if (shaderBlob != nullptr)
	{
		shaderBlob->Release();
	}
	return hr;
}




ID3D11ShaderResourceView* Particle::GetSRVP() const
{
	return m_srv.Get();
}
ID3D11ShaderResourceView* const* Particle::GetSRVPP() const
{
	return m_srv.GetAddressOf();
}

ID3D11UnorderedAccessView* Particle::GetUAVP() const
{
	return m_uav.Get();
}
ID3D11UnorderedAccessView* const* Particle::GetUAVPP() const
{
	return m_uav.GetAddressOf();
}

ID3D11VertexShader* Particle::GetVertexShaderP() const
{
	return m_vShader.Get();
}
ID3D11VertexShader* const* Particle::GetVertexShaderPP() const
{
	return m_vShader.GetAddressOf();
}

ID3D11GeometryShader* Particle::GetGeometryShaderP() const
{
	return m_gShader.Get();
}
ID3D11GeometryShader* const* Particle::GetGeometryShaderPP() const
{
	return m_gShader.GetAddressOf();
}

ID3D11PixelShader* Particle::GetPixelShaderP() const
{
	return m_pShader.Get();
}
ID3D11PixelShader* const* Particle::GetPixelShaderPP() const
{
	return m_pShader.GetAddressOf();
}

ID3D11ComputeShader* Particle::GetComputeShaderP() const
{
	return m_cShader.Get();
}
ID3D11ComputeShader* const* Particle::GetComputeShaderPP() const
{
	return m_cShader.GetAddressOf();
}

UINT Particle::Count() const
{
	return m_particles;
}
bool Particle::IsInitialized() const
{
	return m_initialized;
}
