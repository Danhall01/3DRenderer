#include "Renderer.h"
#include "Settings.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>

constexpr auto MESH_MAX_SIZE = 250'000;
constexpr auto LIGHT_MAX_COUNT = 10;
constexpr auto BYTEWIDTH_VERTEX_MAX = sizeof(Vertex) * MESH_MAX_SIZE;
constexpr auto BYTEWIDTH_INDEX_MAX = sizeof(int) * MESH_MAX_SIZE;
constexpr auto BYTEWIDTH_LIGHT_MAX = sizeof(Light) * LIGHT_MAX_COUNT;

#include <assert.h>
#include <comdef.h>

#include "enables.h"

//Window helper
wWindow Renderer::CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction)
{
	return wWindow(L"Renderer", instance, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, nCmdShow, eventFunction);
}



//Camera Manager
void Renderer::CreateDXCam()
{
	//Some default settings
	m_dxCam = Camera(0, 0, 0, 90.0f, BASE_WINDOW_RATIO, 0.1f, 300.0f);
}
void Renderer::CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_dxCam = Camera(pos, fovDegrees, aspectRatio, nearZ, farZ);
}
void Renderer::CreateDXCam(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_dxCam = Camera(x, y, z, fovDegrees, aspectRatio, nearZ, farZ);
}
const Camera& Renderer::GetDXCamera() const { return m_dxCam; }

void Renderer::AddDXCamPos(float right, float up, float forward)
{
	m_dxCam.AddPosition(right, up, forward);
}
void Renderer::RotateDXCam(float pitch, float yaw, float roll)
{
	m_dxCam.AddRotation(pitch, yaw, roll);
}
void Renderer::UpdateDXCam()
{
	// Aspect ratio change (would require major rebuild)
	// m_dxCam.UpdateProjectionMatrix(data);
	m_dxCam.UpdateViewMatrix();
}

void Renderer::AddLight(const Light& light)
{
	m_lightArr.push_back(light);
}
void Renderer::AddShadowLight(const Light & light, const wWindow& window)
{
	m_shadowlightManager.AddLight(light, window);
}


// Rendering Manager
Renderer::Renderer()
{
	/// Set up camera
	CreateDXCam();
}
Renderer::~Renderer()
{
//#ifdef _DEBUG
//	WRL::ComPtr<ID3D11Debug> debug;
//	m_device.As(&debug);
//	m_hr = debug.Get()->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
//#endif

}

bool Renderer::Build(wWindow window)
{
	// Set up the renderer
	ID3DBlob* shaderBlob = {};
	// D3D11
	if (!BuildDeviceAndSwapChain(window))      { infoDump((unsigned)__LINE__); return false; }
	if (!BuildRenderTargetView())              { infoDump((unsigned)__LINE__); return false; }
	if (!BuildViewport(window))                { infoDump((unsigned)__LINE__); return false; }
	
	// Textures
	if (!BuildTextureBuffer())                 { infoDump((unsigned)__LINE__); return false; }
	if (!BuildSampler())                       { infoDump((unsigned)__LINE__); return false; }
	
	// Buffers
	if (!BuildVertexBuffer())                  { infoDump((unsigned)__LINE__); return false; }
	if (!BuildIndexBuffer())                   { infoDump((unsigned)__LINE__); return false; }
	
	//Cbuffers
	if (!BuildVertexConstantBuffer())          { infoDump((unsigned)__LINE__); return false; }

	//Deferred Build
	if (!BuildShadersDeferred(shaderBlob))     { infoDump((unsigned)__LINE__); return false; }
	if (!BuildInputLayoutDeferred(shaderBlob)) { infoDump((unsigned)__LINE__); return false; }
	if (!BuildUnorderedAccessView(window))     { infoDump((unsigned)__LINE__); return false; }
	if (!BuildGraphBuffer(window))             { infoDump((unsigned)__LINE__); return false; }
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_deferredRTVOutput[i] = m_gbuffer[i].renderTargetView;
		m_deferredSRVInput[i] = m_gbuffer[i].shaderResourceView;
	}
#if LOD
	if (!BuildLOD(shaderBlob))						   { infoDump((unsigned)__LINE__); return false; }
#endif

	//Lighting
	if (!BuildLightBuffer())                   { infoDump((unsigned)__LINE__); return false; }
	
	if (!BuildShadowPass(shaderBlob, window))              { infoDump((unsigned)__LINE__); return false; }


	shaderBlob->Release();
	return true;
}

// D3D11
bool Renderer::BuildDeviceAndSwapChain(const wWindow& window)
{
	/// Swapchain
	DXGI_MODE_DESC displayDesc = {};
	displayDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	displayDesc.Height                  = (UINT)window.GetWindowHeight();
	displayDesc.Width                   = (UINT)window.GetWindowWidth();
	displayDesc.RefreshRate.Numerator   = 144; // Hertz
	displayDesc.RefreshRate.Denominator = 1;
	displayDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	displayDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

	//Anti-aliasing
	DXGI_SAMPLE_DESC multiSamplingDesc = {};
	multiSamplingDesc.Count = 1;
	multiSamplingDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.SampleDesc   = multiSamplingDesc;
	swapChainDesc.BufferDesc   = displayDesc;
	swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	swapChainDesc.BufferCount  = 1;
	swapChainDesc.OutputWindow = window.Data();
	swapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed     = true;
	swapChainDesc.Flags        = 0;

	D3D_FEATURE_LEVEL featureLevel[] = {
		D3D_FEATURE_LEVEL_11_0
	};
	UINT flags = 0;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	m_hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		featureLevel, (sizeof(featureLevel) / sizeof(D3D_FEATURE_LEVEL)),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(),
		NULL, // Gives which of feature levels is supported
		m_immediateContext.GetAddressOf()
	);
	if (FAILED(m_hr))
		return false;
	return true;
}
bool Renderer::BuildRenderTargetView()
{
	/// Render Target View
	ID3D11Resource* backbuffer = nullptr;
	m_hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backbuffer));
	if (FAILED(m_hr))
		return false;

	m_device->CreateRenderTargetView(
		backbuffer,
		NULL, //Mipmap lvl 0
		&m_rtv
	);
	if (FAILED(m_hr))
		return false;

	backbuffer->Release();
	return true;
}
bool Renderer::BuildViewport(const wWindow& window)
{
	/// Viewport
	m_viewport.Height    = window.GetWindowHeight();
	m_viewport.Width     = window.GetWindowWidth();
	m_viewport.MinDepth  = 0;
	m_viewport.MaxDepth  = 1;
	m_viewport.TopLeftX  = 0; // o --
	m_viewport.TopLeftY  = 0; // |

	return true;
}

// Mesh Helper Functions
bool Renderer::BuildVertexConstantBuffer()
{
	/// Vertex CBuffer
	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage               = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	Desc.ByteWidth           = sizeof(WVPMatrix);
	Desc.StructureByteStride = 0;
	Desc.MiscFlags           = 0;

	WVPMatrix matrix = {
		dx::XMMatrixIdentity(),
		dx::XMMatrixIdentity(),
		m_dxCam.GetViewMatrix(),
		m_dxCam.GetProjectionMatrix()
	};
	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem          = &matrix;
	subData.SysMemPitch      = 0;
	subData.SysMemSlicePitch = 0;

	m_hr = m_device->CreateBuffer(
		&Desc,
		&subData,
		m_vConstBuffer.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}
bool Renderer::UpdateVertexConstantBuffer(const Mesh& mesh)
{
	dx::XMVECTOR det = dx::XMMatrixDeterminant(mesh.GetMatrix());
	WVPMatrix wvpMatrix = {
		dx::XMMatrixTranspose(mesh.GetMatrix()),
		dx::XMMatrixInverse(
			&det, 
			dx::XMMatrixTranspose(mesh.GetMatrix())),
		dx::XMMatrixTranspose(m_dxCam.GetViewMatrix()),
		dx::XMMatrixTranspose(m_dxCam.GetProjectionMatrix())
	};

	D3D11_MAPPED_SUBRESOURCE mResource = {};

	m_hr = m_immediateContext->Map(m_vConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	if (FAILED(m_hr))
		return false;
	memcpy(
		mResource.pData,
		&wvpMatrix,
		sizeof(wvpMatrix)
	);
	m_immediateContext->Unmap(m_vConstBuffer.Get(), 0);

	return true;
}
bool Renderer::UpdateVertexConstantBuffer(dx::XMMATRIX& matrix)
{
	dx::XMVECTOR det = dx::XMMatrixDeterminant(matrix);
	WVPMatrix wvpMatrix = {
		dx::XMMatrixTranspose(matrix),
		dx::XMMatrixInverse(
				&det,
				matrix
        ),
		dx::XMMatrixTranspose(m_dxCam.GetViewMatrix()),
		dx::XMMatrixTranspose(m_dxCam.GetProjectionMatrix())
	};

	D3D11_MAPPED_SUBRESOURCE mResource = {};

	m_hr = m_immediateContext->Map(m_vConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	if (FAILED(m_hr))
		return false;
	memcpy(
		mResource.pData,
		&wvpMatrix,
		sizeof(wvpMatrix)
	);
	m_immediateContext->Unmap(m_vConstBuffer.Get(), 0);

	return true;
}
bool Renderer::UpdateVertexConstantBuffer(dx::XMMATRIX& modelView, const Camera& cam)
{
	dx::XMVECTOR det = dx::XMMatrixDeterminant(modelView);
	WVPMatrix wvpMatrix = {
		dx::XMMatrixTranspose(modelView),
		dx::XMMatrixInverse(&det, modelView), // normal world matrix
		dx::XMMatrixTranspose(cam.GetViewMatrix()),
		dx::XMMatrixTranspose(cam.GetProjectionMatrix())
	};

	D3D11_MAPPED_SUBRESOURCE mResource = {};

	m_hr = m_immediateContext->Map(m_vConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	if (FAILED(m_hr))
		return false;
	memcpy(
		mResource.pData,
		&wvpMatrix,
		sizeof(wvpMatrix)
	);
	m_immediateContext->Unmap(m_vConstBuffer.Get(), 0);

	return true;
}

// Texture Helper Functions
bool Renderer::BuildTextureBuffer()
{
	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.ByteWidth = sizeof(TextureData);
	Desc.StructureByteStride = 0;
	Desc.MiscFlags = 0;

	m_hr = m_device->CreateBuffer(
		&Desc,
		NULL,
		m_materialBuffer.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}
bool Renderer::UpdateTextureBuffer(const TextureData& texture)
{
	D3D11_MAPPED_SUBRESOURCE mResource = {};

	m_hr = m_immediateContext->Map(m_materialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	if (FAILED(m_hr))
		return false;
	memcpy(
		mResource.pData,
		&texture,
		sizeof(TextureData)
	);
	m_immediateContext->Unmap(m_materialBuffer.Get(), 0);

	return true;
}
bool Renderer::BuildSampler()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;


	m_hr = m_device->CreateSamplerState(
		&samplerDesc,
		&m_samplerState
	);

	return SUCCEEDED(m_hr);
}

// Lighting
bool Renderer::BuildLightBuffer()
{
	// Build SRV for light
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth           = BYTEWIDTH_LIGHT_MAX;
	bufferDesc.StructureByteStride = sizeof(Light);
	bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	m_hr = m_device->CreateBuffer(
		&bufferDesc,
		NULL,
		m_lightBuffer.GetAddressOf());
	if (FAILED(m_hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = LIGHT_MAX_COUNT;

	m_hr = m_device->CreateShaderResourceView(
		m_lightBuffer.Get(),
		&srvDesc,
		m_deferredSRVInput[BUFFER_COUNT].GetAddressOf());
	if (FAILED(m_hr))
		return false;


	// Build cBuffer for count
	D3D11_BUFFER_DESC cbufferDesc = {};
	cbufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
	cbufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.ByteWidth           = sizeof(LightCData);
	cbufferDesc.StructureByteStride = 0;
	cbufferDesc.MiscFlags           = 0;

	LightCData data = {};
	data.camPos_count.fill(0);

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem          = &data;
	subData.SysMemPitch      = 0;
	subData.SysMemSlicePitch = 0;

	m_hr = m_device->CreateBuffer(
		&cbufferDesc,
		&subData,
		m_lightCount.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}
bool Renderer::UpdateLighting()
{
	std::vector<Light> allLight = m_lightArr;
	std::vector<Light> addVec = m_shadowlightManager.GetLightVec(); // Very suboptimal way to do it
	for (int i = 0; i < addVec.size(); i++)
	{
		allLight.push_back(addVec[i]);
	}
	
	if (allLight.size() > LIGHT_MAX_COUNT)
		return false;


	// Update the matrixes
	int shadowLightAmount = 0;
	for ( auto& light : allLight )
	{
		if (light.CosOuter_Inner_SMap_count[2] == 1)
		{
			light.shadowVPMatrix = m_shadowlightManager.GetLightVPMatrix(shadowLightAmount);
			++shadowLightAmount;
		}
		else
		{
			light.shadowVPMatrix = dx::XMMatrixIdentity();
		}
	}


	// Update lighting buffer
	D3D11_MAPPED_SUBRESOURCE m_ResourceSRV = {};
	m_hr = m_immediateContext->Map(m_lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m_ResourceSRV);
	if (FAILED(m_hr))
		return false;
	memcpy(
		m_ResourceSRV.pData,
		allLight.data(),
		sizeof(Light) * allLight.size()
	);
	m_immediateContext->Unmap(m_lightBuffer.Get(), 0);



	// Update lightCount cbuffer
	D3D11_MAPPED_SUBRESOURCE mResourceCbuffer = {};
	LightCData lightData = {};
	lightData.camPos_count[0] = static_cast<UINT>(m_dxCam.GetPositionFloat3().x);
	lightData.camPos_count[1] = static_cast<UINT>(m_dxCam.GetPositionFloat3().y);
	lightData.camPos_count[2] = static_cast<UINT>(m_dxCam.GetPositionFloat3().z);
	lightData.camPos_count[3] = static_cast<UINT>(allLight.size());

	m_hr = m_immediateContext->Map(m_lightCount.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResourceCbuffer);
	if (FAILED(m_hr))
		return false;
	memcpy(
		mResourceCbuffer.pData,
		&lightData,
		sizeof(LightCData)
	);
	m_immediateContext->Unmap(m_lightCount.Get(), 0);
	return true;
}

bool Renderer::BuildShadowPass(ID3DBlob* shaderBlob, wWindow window)
{
	// Create the manager
	m_hr = m_shadowlightManager.Init(m_device.Get(), window);
	if (FAILED(m_hr)) 
	{
		return false;
	}

#if _DEBUG
	wchar_t vsPath[] = L"../x64/Debug/ShadowPassVS.cso";
#else
	wchar_t vsPath[] = L"../x64/Release/ShadowPassVS.cso";
#endif

	// Compile the shadowpass vertex shader
	D3DReadFileToBlob(vsPath, &shaderBlob);
	if (FAILED(m_hr))
		return false;

	m_hr = m_device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL,
		m_shadowVertexShader.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}

bool Renderer::UpdateLODCBuffer(const Mesh& mesh, float tesFactor)
{
	// Mesh position: 
	// From the world matrix:
	// 0 0 0 0
	// 0 0 0 0
	// 0 0 0 0
	// x y z 0
	dx::XMVECTOR meshScale = {};
	dx::XMVECTOR meshRotQuat = {};

	dx::XMVECTOR meshTranslation = {};
	if (!dx::XMMatrixDecompose(&meshScale, &meshRotQuat, &meshTranslation, mesh.GetMatrix()))
	{
		return false;
	}
	dx::XMFLOAT3 meshPos = {};
	dx::XMStoreFloat3(&meshPos, meshTranslation);

	// Cam pos
	dx::XMFLOAT3 camPos = m_dxCam.GetPositionFloat3();
	

	// Upload data
	LODCBufferData newData = {
		{camPos.x,  camPos.y,  camPos.z,  0.0f},
		{meshPos.x, meshPos.y, meshPos.z, 0.0f},
		{tesFactor, tesFactor, tesFactor, tesFactor}
	};
	D3D11_MAPPED_SUBRESOURCE m_ResourceSRV = {};
	m_hr = m_immediateContext->Map(m_LODCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m_ResourceSRV);
	if (FAILED(m_hr))
		return false;
	memcpy(
		m_ResourceSRV.pData,
		&newData,
		sizeof(newData)
	);
	m_immediateContext->Unmap(m_LODCBuffer.Get(), 0);
	return SUCCEEDED(m_hr);
}


// Buffers
bool Renderer::BuildVertexBuffer()
{
	/// Vertex Buffer
	D3D11_BUFFER_DESC vertexDesc = {};
	vertexDesc.Usage               = D3D11_USAGE_DYNAMIC;
	vertexDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	vertexDesc.ByteWidth           = BYTEWIDTH_VERTEX_MAX;
	vertexDesc.StructureByteStride = 0;
	vertexDesc.MiscFlags           = 0;

	m_hr = m_device->CreateBuffer(
		&vertexDesc,
		NULL, // Size only
		m_vertexBuffer.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}
bool Renderer::BuildIndexBuffer()
{
	/// Index Buffer
	D3D11_BUFFER_DESC indexDesc = {};
	indexDesc.Usage               = D3D11_USAGE_DYNAMIC;
	indexDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	indexDesc.ByteWidth           = BYTEWIDTH_INDEX_MAX;
	indexDesc.StructureByteStride = 0;
	indexDesc.MiscFlags           = 0;

	m_hr = m_device->CreateBuffer(
		&indexDesc,
		NULL, // Size only
		m_indexBuffer.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}
bool Renderer::UpdateVertexBuffer(Mesh& mesh)
{
#if _DEBUG
	assert(mesh.GetVertexByteWidth() < BYTEWIDTH_VERTEX_MAX);
#endif
	D3D11_MAPPED_SUBRESOURCE mResource = {};
	// Take pointer from GPU
	m_hr = m_immediateContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	memcpy(
		mResource.pData,
		mesh.GetVertexData(),
		mesh.GetVertexByteWidth()
	);

	// Give pointer back to GPU
	m_immediateContext->Unmap(m_vertexBuffer.Get(), 0);
	return SUCCEEDED(m_hr);
}
bool Renderer::UpdateIndexBuffer(Mesh& mesh)
{
#if _DEBUG
	assert(mesh.GetIndexByteWidth() < BYTEWIDTH_INDEX_MAX);
#endif 
	D3D11_MAPPED_SUBRESOURCE mResource = {};
	// Take pointer from GPU
	m_hr = m_immediateContext->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	memcpy(
		mResource.pData,
		mesh.GetIndexData(),
		mesh.GetIndexByteWidth()
	);

	// Give pointer back to GPU
	m_immediateContext->Unmap(m_indexBuffer.Get(), 0);

	return SUCCEEDED(m_hr);
}

// Deferred Rendering
bool Renderer::BuildShadersDeferred(ID3DBlob*& out_shaderBlob)
{
	ShaderSet shaders = {};
	shaders.geometryShader = nullptr;
	shaders.hullShader     = nullptr;
	shaders.domainShader   = nullptr;
#if _DEBUG
	wchar_t pxPath[] = L"../x64/Debug/PixelShaderDeferred.cso";
	wchar_t vsPath[] = L"../x64/Debug/VertexShaderDeferred.cso";
	wchar_t csPath[] = L"../x64/Debug/LightPass.cso";
#else
	wchar_t pxPath[] = L"../x64/Release/PixelShaderDeferred.cso";
	wchar_t vsPath[] = L"../x64/Release/VertexShaderDeferred.cso";
	wchar_t csPath[] = L"../x64/Release/LightPass.cso";
#endif
	m_hr = D3DReadFileToBlob(csPath, &out_shaderBlob);
	if (FAILED(m_hr))
		return false;
	m_hr = m_device->CreateComputeShader(
		out_shaderBlob->GetBufferPointer(),
		out_shaderBlob->GetBufferSize(),
		NULL,
		shaders.computeShader.GetAddressOf()
	);
	if (FAILED(m_hr))
		return false;

	// Pixel Shader
	m_hr = D3DReadFileToBlob(pxPath, &out_shaderBlob);
	if (FAILED(m_hr))
		return false;
	m_hr = m_device->CreatePixelShader(
		out_shaderBlob->GetBufferPointer(),
		out_shaderBlob->GetBufferSize(),
		NULL,
		shaders.pixelShader.GetAddressOf()
	);
	if (FAILED(m_hr))
		return false;

	//Vertex Shader
	D3DReadFileToBlob(vsPath, &out_shaderBlob);
	if (FAILED(m_hr))
		return false;
	m_hr = m_device->CreateVertexShader(
		out_shaderBlob->GetBufferPointer(),
		out_shaderBlob->GetBufferSize(),
		NULL,
		shaders.vertexShader.GetAddressOf()
	);

	m_shaders.push_back(shaders);
	return SUCCEEDED(m_hr);
}
bool Renderer::BuildInputLayoutDeferred(ID3DBlob*& shaderBlob)
{
	/// Inputlayout
	ID3D11InputLayout* defaultLayout;
	D3D11_INPUT_ELEMENT_DESC vShaderInput[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV"      , 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	m_hr = m_device->CreateInputLayout(
		vShaderInput, (UINT)(sizeof(vShaderInput) / sizeof(*vShaderInput)),
		shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
		&defaultLayout
	);
	if (FAILED(m_hr))
		return false;
	
	m_inputLayout.push_back(defaultLayout);
	defaultLayout->Release();
	return true;
}
bool Renderer::BuildGraphBuffer(const wWindow& window)
{
	// Texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width              = static_cast<UINT>(window.GetWindowWidth());
	textureDesc.Height             = static_cast<UINT>(window.GetWindowHeight());
	textureDesc.MipLevels          = 1;
	textureDesc.ArraySize          = 1;
	textureDesc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage              = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_hr = m_device->CreateTexture2D(
			&textureDesc, 
			NULL, 
			m_gbuffer[i].texture.GetAddressOf());
		if (FAILED(m_hr))
			return false;
	}
	
	// RenderTargetView
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format        = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_hr = m_device->CreateRenderTargetView(
			m_gbuffer[i].texture.Get(),
			&rtvDesc,
			m_gbuffer[i].renderTargetView.GetAddressOf());
		if (FAILED(m_hr))
			return false;
	}

	// ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format              = textureDesc.Format;
	srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_hr = m_device->CreateShaderResourceView(
			m_gbuffer[i].texture.Get(),
			&srvDesc,
			m_gbuffer[i].shaderResourceView.GetAddressOf());
		if (FAILED(m_hr))
			return false;
	}

	// DepthStencilView
	WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
	depthStencilBufferDesc.Width              = static_cast<UINT>(window.GetWindowWidth());
	depthStencilBufferDesc.Height             = static_cast<UINT>(window.GetWindowHeight());
	depthStencilBufferDesc.MipLevels          = 1;
	depthStencilBufferDesc.ArraySize          = 1;
	depthStencilBufferDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count   = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	m_hr = m_device->CreateTexture2D(&depthStencilBufferDesc, NULL, depthStencilTexture.GetAddressOf());
	if (FAILED(m_hr))
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format        = depthStencilBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_hr = m_device->CreateDepthStencilView(depthStencilTexture.Get(), &depthStencilViewDesc, m_dsv.GetAddressOf());

	return SUCCEEDED(m_hr);
}
bool Renderer::BuildUnorderedAccessView(const wWindow& window)
{
	if (m_uav.size() > 0)
		return false;
	WRL::ComPtr<ID3D11UnorderedAccessView> tempUAV;
	WRL::ComPtr<ID3D11Resource> backbuffer;
	m_hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(backbuffer.GetAddressOf()));
	if (FAILED(m_hr))
		return false;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	m_hr = m_device->CreateUnorderedAccessView(
		backbuffer.Get(),
		&UAVDesc,
		tempUAV.GetAddressOf()
	);
	m_uav.push_back(tempUAV);
	return SUCCEEDED(m_hr);
}

// LOD
bool Renderer::BuildLOD(ID3DBlob* shaderBlob)
{
#if _DEBUG
	wchar_t hsPath[] = L"../x64/Debug/LODHullShader.cso";
	wchar_t dsPath[] = L"../x64/Debug/LODDomainShader.cso";
#else
	wchar_t hsPath[] = L"../x64/Release/LODHullShader.cso";
	wchar_t dsPath[] = L"../x64/Release/LODDomainShader.cso";
#endif


	// Hull shader
	m_hr = D3DReadFileToBlob(hsPath, &shaderBlob);
	if (FAILED(m_hr))
		return false;
	m_hr = m_device->CreateHullShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL,
		m_shaders[0].hullShader.GetAddressOf()
	);
	if (FAILED(m_hr))
		return false;

	// Domain Shader
	m_hr = D3DReadFileToBlob(dsPath, &shaderBlob);
	if (FAILED(m_hr))
		return false;
	m_hr = m_device->CreateDomainShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		NULL,
		m_shaders[0].domainShader.GetAddressOf()
	);
	if (FAILED(m_hr))
		return false;


	//Cbuffer: 
	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage               = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	Desc.ByteWidth           = sizeof(LODCBufferData);
	Desc.StructureByteStride = 0;
	Desc.MiscFlags           = 0;

	LODCBufferData data = {
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	};

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem          = &data;
	subData.SysMemPitch      = 0;
	subData.SysMemSlicePitch = 0;

	m_hr = m_device->CreateBuffer(
		&Desc,
		&subData,
		m_LODCBuffer.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}


void Renderer::infoDump(unsigned line)
{
#if _DEBUG
	// Gets the last known error and throws
	_com_error error(m_hr);
	LPCTSTR errorText = error.ErrorMessage();
	_wassert(errorText, _CRT_WIDE(__FILE__), line);
#else
	assert(1);
#endif

}


//Obj Parser
bool Renderer::ParseObj(std::vector<std::string> pathFiles, unsigned char flag)
{
	/// Parses the files included as parameter
	/// !OBS: The first input in the array/vector should always be equal to the 
	/// path of the files and empty if files are located in the same folder.
	if (pathFiles.size() < 2)
		return false;

	Assets asset = {};
	for (int i = 1; i < pathFiles.size(); i++)
	{
		if (!asset.ParseFromObjFile(pathFiles[0], pathFiles[i], false))
			return false;
	}
	m_assets.try_emplace(flag, asset);
	asset.Clear();
	UpdateImageMap();
	return true;
}
bool Renderer::SetMeshMatrix(std::string id, const dx::XMMATRIX& matrix)
{
	Mesh tempMesh = {};
	for (auto& asset : m_assets)
	{
		if (asset.second.GetMesh(id, tempMesh))
		{
			tempMesh.SetMatrix(
				matrix
			);
			return true;
		}
	}

	return false;
}


void Renderer::DrawDeferred(std::vector< std::pair<std::string, dx::XMMATRIX> >& drawTargets, const wWindow& window)
{
	ClearBuffer();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//Once per present
#if LOD
	m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
#else
	m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#endif
	m_immediateContext->RSSetViewports(1, &m_viewport);
	m_immediateContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	// Clear input textures
	ID3D11ShaderResourceView* nullSRV[BUFFER_COUNT+2] = { nullptr };
	m_immediateContext->CSSetShaderResources(0, BUFFER_COUNT+2, nullSRV);

	// OM
	m_immediateContext->OMSetRenderTargets(BUFFER_COUNT, m_deferredRTVOutput[0].GetAddressOf(), m_dsv.Get());

	for (auto& asset : m_assets)
	{
		// Read asset flag for used data
		unsigned char data = asset.first;
		int shaderSet = (int)(data & 0b00001111);
		int inputlayout = (int)((data & 0b11110000) >> 4);
		// Shaders
		m_immediateContext->VSSetShader(m_shaders[shaderSet].vertexShader.Get()  , nullptr, 0);
		m_immediateContext->HSSetShader(m_shaders[shaderSet].hullShader.Get()    , nullptr, 0);
		m_immediateContext->DSSetShader(m_shaders[shaderSet].domainShader.Get()  , nullptr, 0);
		m_immediateContext->GSSetShader(m_shaders[shaderSet].geometryShader.Get(), nullptr, 0);
		m_immediateContext->PSSetShader(m_shaders[shaderSet].pixelShader.Get()   , nullptr, 0);
		// IA
		m_immediateContext->IASetInputLayout(m_inputLayout[inputlayout].Get());

		RenderDrawTargets(asset.second, stride, offset, drawTargets);
	}
	// Unbind output textures
	m_immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
	// CS
	m_immediateContext->CSSetShader(m_shaders[0].computeShader.Get(), nullptr, 0);
	m_immediateContext->CSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	m_immediateContext->CSSetUnorderedAccessViews(0, 1, m_uav[0].GetAddressOf(), 0);
	m_immediateContext->CSSetShaderResources(0, (BUFFER_COUNT + 1), m_deferredSRVInput[0].GetAddressOf());
	m_immediateContext->CSSetConstantBuffers(0, 1, m_lightCount.GetAddressOf());


	//Added shadows
	// shadow sampler bind
	m_immediateContext->CSSetSamplers(1, 1, m_shadowlightManager.GetShadowSamplerPP());
	m_immediateContext->CSSetShaderResources((BUFFER_COUNT + 1), 1, m_shadowlightManager.GetSRVPP());

	// shadow srv bind



	// End added shadows

	m_immediateContext->Dispatch(
		static_cast<int>(window.GetWindowWidth() / 16), 
		static_cast<int>(window.GetWindowHeight() / 16),
		1);
}
void Renderer::ShadowPass(std::vector< std::pair<std::string, dx::XMMATRIX> >& drawTargets)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	// Basic setup
	m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_immediateContext->VSSetShader(m_shadowVertexShader.Get(), nullptr, 0);
	
	//Unbind shaders
	m_immediateContext->HSSetShader(nullptr, nullptr, 0);
	m_immediateContext->DSSetShader(nullptr, nullptr, 0);
	m_immediateContext->GSSetShader(nullptr, nullptr, 0);
	m_immediateContext->PSSetShader(nullptr, nullptr, 0);
	m_immediateContext->CSSetShader(nullptr, nullptr, 0);

	//Unbind srv conected to RTV
	ID3D11ShaderResourceView* nullSRV[BUFFER_COUNT + 2] = { nullptr };
	m_immediateContext->CSSetShaderResources(0, BUFFER_COUNT + 2, nullSRV);


	m_immediateContext->RSSetViewports(1, &m_viewport);

	for (UINT lightIndex = 0; lightIndex < m_shadowlightManager.Length(); ++lightIndex) 
	{
		ID3D11DepthStencilView* dsView = m_shadowlightManager.GetDSVP(lightIndex);
		m_immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1, 0);
		m_immediateContext->OMSetRenderTargets(0, nullptr, dsView);

		//Render loop
		for (auto& asset : m_assets)
		{
			unsigned char data = asset.first;
			int inputlayout = (int)((data & 0b11110000) >> 4);
			m_immediateContext->IASetInputLayout(m_inputLayout[inputlayout].Get());

			Mesh mesh = {};
			for (int i = 0; i < drawTargets.size(); i++)
			{
				if (asset.second.GetMesh(drawTargets[i].first, mesh))
				{
					//Per mesh updates
					if ( !UpdateVertexConstantBuffer(drawTargets[i].second, m_shadowlightManager.GetDXCamera(lightIndex)) ) { infoDump((unsigned)__LINE__); return; }
					m_immediateContext->VSSetConstantBuffers(0, 1, m_vConstBuffer.GetAddressOf());


					if (!UpdateVertexBuffer(mesh)) { infoDump((unsigned)__LINE__); return; }
					if (!UpdateIndexBuffer(mesh)) { infoDump((unsigned)__LINE__); return; }
					m_immediateContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
					m_immediateContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

					for (auto& submesh : mesh.GetSubmeshMap())
					{
						//Drawcall
						m_immediateContext->DrawIndexed(
							submesh.second.indiceCount,
							submesh.second.indiceStartIndex,
							submesh.second.verticeStartIndex
						);
					}
				}
			}

		}
	}

	m_immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
}


void Renderer::Render()
{
	m_swapChain->Present(1, 0);
};
void Renderer::ClearBuffer()
{
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0 };
	m_immediateContext->ClearRenderTargetView(m_rtv.Get(), bgColor);
	m_immediateContext->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	m_immediateContext->ClearUnorderedAccessViewFloat(m_uav[0].Get(), &bgColor[0]);

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		m_immediateContext->ClearRenderTargetView(m_gbuffer[i].renderTargetView.Get(), bgColor);
	}
}
void Renderer::RenderDrawTargets(const Assets& currentAsset,
	const UINT& stride, const UINT& offset,
	std::vector< std::pair<std::string, dx::XMMATRIX> >& drawTargets)
{
	ID3D11ShaderResourceView* textureSRV[3] = {};
	Mesh mesh = {};
	for (int i = 0; i < drawTargets.size(); i++)
	{
		if (currentAsset.GetMesh(drawTargets[i].first, mesh)) // TODO: Polish, redundent loop
		{
			//Per mesh updates
			if (!UpdateVertexConstantBuffer(drawTargets[i].second)) { infoDump((unsigned)__LINE__); return; }
			m_immediateContext->VSSetConstantBuffers(0, 1, m_vConstBuffer.GetAddressOf());

			if (!UpdateVertexBuffer(mesh)) { infoDump((unsigned)__LINE__); return; }
			if (!UpdateIndexBuffer(mesh)) { infoDump((unsigned)__LINE__); return; }
			m_immediateContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
			m_immediateContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

#if LOD
			if (!UpdateLODCBuffer(mesh, 32.0f)) { infoDump((unsigned)__LINE__); return; }
			m_immediateContext->HSSetConstantBuffers(0, 1, m_LODCBuffer.GetAddressOf());
			m_immediateContext->DSSetConstantBuffers(0, 1, m_vConstBuffer.GetAddressOf());
#endif

			Texture tex = {};
			for (auto& submesh : mesh.GetSubmeshMap())
			{
				//Per submesh updates
				currentAsset.GetTexture(submesh.second.textureId, tex);
				UpdateTextureBuffer(tex.Data());

				// update sampler data
				textureSRV[0] = m_loadedImages.at(tex.GetImageKa()).Get();
				textureSRV[1] = m_loadedImages.at(tex.GetImageKd()).Get();
				textureSRV[2] = m_loadedImages.at(tex.GetImageKs()).Get();


				m_immediateContext->PSSetConstantBuffers(0, 1, m_materialBuffer.GetAddressOf());
				m_immediateContext->PSSetShaderResources(0, 3, textureSRV);

				//Drawcall
				m_immediateContext->DrawIndexed(
					submesh.second.indiceCount,
					submesh.second.indiceStartIndex,
					submesh.second.verticeStartIndex
				);
			}
		}
	}
}

bool Renderer::UpdateImageMap()
{
	for (auto& asset : m_assets)
	{
		for (auto& img : asset.second.GetImageMap())
		{
			if (m_loadedImages.count(img.first) > 0)
				continue;
			WRL::ComPtr<ID3D11Texture2D> pTexture;
			WRL::ComPtr<ID3D11ShaderResourceView> srv;

			DXGI_FORMAT format = {};
			switch (img.second.channels)
			{
				case 1: format = DXGI_FORMAT_R8_UNORM; break;
				case 2: format = DXGI_FORMAT_R8G8_UNORM; break;
				case 3: // Redundant
				case 4: format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
			}
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Height = img.second.height;
			textureDesc.Width = img.second.width;
			textureDesc.Format = format;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subData = {};
			subData.pSysMem = img.second.img;
			subData.SysMemPitch = static_cast<UINT>(img.second.width * img.second.channels);
			subData.SysMemSlicePitch = 0;

			m_hr = m_device->CreateTexture2D(
				&textureDesc,
				&subData,
				pTexture.GetAddressOf()
			);
			if (FAILED(m_hr))
				return false;


			// Create the SRV for the image
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;

			m_hr = m_device->CreateShaderResourceView(
				pTexture.Get(),
				&srvDesc,
				srv.GetAddressOf()
			);
			if (FAILED(m_hr))
				return false;

			m_loadedImages.emplace(img.first, srv);
		}
	}
	return true;
}
