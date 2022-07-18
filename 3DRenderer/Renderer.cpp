#include "Renderer.h"
#include "Settings.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>
#define BYTEWIDTH_MAX D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT

#include <assert.h>
#include <comdef.h>


//Window helper
wWindow Renderer::CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction)
{
	return wWindow(L"Renderer", instance, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, nCmdShow, eventFunction);
}


//Camera Manager
void Renderer::CreateDXCam()
{
	//Some default settings
	m_dxCam = Camera(0, 0, 0, 90.0f, BASE_WINDOW_RATIO, 0.1f, 100.0f);
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
void Renderer::AddDXCamPos(float x, float y, float z)
{
	m_dxCam.AddPosition(x, y, z);
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
	if (!BuildSwapChain(window)) { infoDump((unsigned)__LINE__); return false; }
	if (!BuildRenderTargetView()) { infoDump((unsigned)__LINE__); return false; }
	if (!BuildViewport(window)) { infoDump((unsigned)__LINE__); return false; }
	
	if (!BuildShadersDefault(shaderBlob)) { infoDump((unsigned)__LINE__); return false; }
	if (!BuildInputLayoutDefault(shaderBlob)) { infoDump((unsigned)__LINE__); return false; }

	if (!BuildVertexConstantBuffer()) { infoDump((unsigned)__LINE__); return false; }

	if (!BuildVertexBuffer()) { infoDump((unsigned)__LINE__); return false; }
	if (!BuildIndexBuffer()) { infoDump((unsigned)__LINE__); return false; }

	shaderBlob->Release();

	return true;
}

bool Renderer::BuildSwapChain(wWindow window)
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
	swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
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
		m_dContext.GetAddressOf()
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
bool Renderer::BuildViewport(wWindow window)
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
bool Renderer::BuildShadersDefault(ID3DBlob*& out_shaderBlob)
{
	shaderSet shaders = {};
	shaders.geometryShader = nullptr;
	shaders.hullShader     = nullptr;
	shaders.domainShader   = nullptr;
	shaders.computeShader  = nullptr;
#if _DEBUG
	wchar_t pxPath[] = L"../x64/Debug/PixelShader.cso";
	wchar_t vsPath[] = L"../x64/Debug/VertexShader.cso";
#else
	wchar_t pxPath[] = L"../x64/Release/PixelShader.sco";
	wchar_t vsPath[] = L"../x64/Release/VertexShader.cso";
#endif

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
bool Renderer::BuildInputLayoutDefault(ID3DBlob*& relativeSBlob)
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
		relativeSBlob->GetBufferPointer(), relativeSBlob->GetBufferSize(),
		&defaultLayout
	);
	if (FAILED(m_hr))
		return false;
	
	m_inputLayout.push_back(defaultLayout);
	defaultLayout->Release();
	return true;
}

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
	WVPMatrix wvpMatrix = {
		dx::XMMatrixTranspose(mesh.GetMatrix()),
		dx::XMMatrixTranspose(m_dxCam.GetViewMatrix()),
		dx::XMMatrixTranspose(m_dxCam.GetProjectionMatrix())
	};

	D3D11_MAPPED_SUBRESOURCE mResource = {};

	m_hr = m_dContext->Map(m_vConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	if (FAILED(m_hr))
		return false;
	memcpy(
		mResource.pData,
		&wvpMatrix,
		sizeof(WVPMatrix)
	);
	m_dContext->Unmap(m_vConstBuffer.Get(), 0);

	return true;
}
bool Renderer::UpdateVertexConstantBuffer(dx::XMMATRIX& matrix)
{
	WVPMatrix wvpMatrix = {
		dx::XMMatrixTranspose(matrix),
		dx::XMMatrixTranspose(m_dxCam.GetViewMatrix()),
		dx::XMMatrixTranspose(m_dxCam.GetProjectionMatrix())
	};

	D3D11_MAPPED_SUBRESOURCE mResource = {};

	m_hr = m_dContext->Map(m_vConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	if (FAILED(m_hr))
		return false;
	memcpy(
		mResource.pData,
		&wvpMatrix,
		sizeof(WVPMatrix)
	);
	m_dContext->Unmap(m_vConstBuffer.Get(), 0);

	return true;
}

bool Renderer::BuildVertexBuffer()
{
	/// Vertex Buffer
	D3D11_BUFFER_DESC vertexDesc = {};
	vertexDesc.Usage               = D3D11_USAGE_DYNAMIC;
	vertexDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	vertexDesc.ByteWidth           = BYTEWIDTH_MAX;
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
	indexDesc.ByteWidth           = BYTEWIDTH_MAX;
	indexDesc.StructureByteStride = 0;
	indexDesc.MiscFlags           = 0;

	m_hr = m_device->CreateBuffer(
		&indexDesc,
		NULL, // Size only
		m_indexBuffer.GetAddressOf()
	);
	return SUCCEEDED(m_hr);
}
bool Renderer::UpdateVertexBuffer(Assets& asset)
{
	D3D11_MAPPED_SUBRESOURCE mResource = {};
	// Take pointer from GPU
	m_hr = m_dContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);

	memcpy(
		mResource.pData,
		asset.GetVertexData(),
		asset.GetVertexByteWidth()
	);

	// Give pointer back to GPU
	m_dContext->Unmap(m_vertexBuffer.Get(), 0);
	return SUCCEEDED(m_hr);
}
bool Renderer::UpdateIndexBuffer(Assets& asset)
{
	D3D11_MAPPED_SUBRESOURCE mResource = {};
	// Take pointer from GPU
	m_hr = m_dContext->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);

	memcpy(
		mResource.pData,
		asset.GetIndexData(),
		asset.GetIndexByteWidth()
	);

	// Give pointer back to GPU
	m_dContext->Unmap(m_indexBuffer.Get(), 0);

	return SUCCEEDED(m_hr);
}

inline void Renderer::infoDump(unsigned line)
{
	// Gets the last known error and throws
	_com_error error(m_hr);
	LPCTSTR errorText = error.ErrorMessage();
	_wassert(errorText, _CRT_WIDE(__FILE__), line);
}


//Obj Parser
bool Renderer::ParseObj(std::vector<std::string> pathFiles, unsigned char flag)
{
	/// Parses the files included as parameter
	/// !OBS: The first input in the array/vector should always be equal to the 
	/// path of the files and empty if files are located in the same folder.
	
	Assets asset = {};

	for (int i = 1; i < pathFiles.size(); i++)
	{
		if (!asset.ParseFromObjFile(pathFiles[0], pathFiles[i], false))
			return false;
	}
	m_assets.try_emplace(flag, asset);
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

//Renderer
void Renderer::Draw(std::vector<std::string> drawTargets)
{
	//TODO
}
void Renderer::Draw(std::vector< std::pair<std::string, dx::XMMATRIX> > drawTargets)
{
	ClearBuffer();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	unsigned char flag = 0; // Default
	// Go through assets
	for (auto& asset : m_assets)
	{
		// Read asset flag for used data
		flag = asset.first;
		int inputlayout = (int)((flag & 0b11110000) >> 4);
		int shaderSet = (int)(flag & 0b00001111);


		if (!UpdateIndexBuffer(asset.second)) { infoDump((unsigned)__LINE__); return; }
		if (!UpdateVertexBuffer(asset.second)) { infoDump((unsigned)__LINE__); return; }


		// Set Shaders
		m_dContext->VSSetShader(m_shaders[shaderSet].vertexShader.Get(), nullptr, 0);
		m_dContext->HSSetShader(m_shaders[shaderSet].hullShader.Get(), nullptr, 0);
		m_dContext->DSSetShader(m_shaders[shaderSet].domainShader.Get(), nullptr, 0);
		m_dContext->GSSetShader(m_shaders[shaderSet].geometryShader.Get(), nullptr, 0);
		m_dContext->PSSetShader(m_shaders[shaderSet].pixelShader.Get(), nullptr, 0);
		m_dContext->CSSetShader(m_shaders[shaderSet].computeShader.Get(), nullptr, 0);

		//IA
		m_dContext->IASetInputLayout(m_inputLayout[inputlayout].Get());
		m_dContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_dContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); // UINT == 32 bit
		m_dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



		//VS


		//RS
		m_dContext->RSSetViewports(1, &m_viewport);

		//PS
		    // Set Light constant buffer

		//OM
		m_dContext->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());


		// Draw all the used meshes in asset
		Mesh mesh = {};
		for (int i = 0; i < drawTargets.size(); i++)
		{
			if (asset.second.GetMesh(drawTargets[i].first, mesh))
			{
				//Per mesh updates
				if (!UpdateVertexConstantBuffer(drawTargets[i].second)) { infoDump((unsigned)__LINE__); return; }
				m_dContext->VSSetConstantBuffers(0, 1, m_vConstBuffer.GetAddressOf());
				// Set constant buffer with material data and texture data (PS buffer)


				//Drawcall
				m_dContext->DrawIndexed(
					mesh.GetIndiceCount(),
					mesh.GetIndiceStartIndex(),
					mesh.GetVerticeStartIndex()
				);
			}
		}
	}
	m_swapChain->Present(1, 0);
};
void Renderer::ClearBuffer()
{
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0 };
	m_dContext->ClearRenderTargetView(m_rtv.Get(), bgColor);
}