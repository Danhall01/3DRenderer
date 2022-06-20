#include "Renderer.h"
#include "Settings.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>



//Window helper
wWindow Renderer::CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction)
{
	return wWindow(L"Renderer", instance, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, nCmdShow, eventFunction);
}


//Camera Manager
void Renderer::CreateDXCam()
{
	//Some default settings
	m_dxCam = Camera(0, 0, 0, 90, BASE_WINDOW_RATIO, 0.1f, 100.0f);
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
#ifdef _DEBUG
	//WRL::ComPtr<ID3D11Debug> debug;
	//m_device.As(&debug);
	//hr = debug.Get()->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_SUMMARY);
#endif

}

bool Renderer::Build(wWindow window)
{
	// Set up the renderer


	if (!BuildSwapChain(window)) { return false; }
	if (!BuildRenderTargetView()) { return false; }
	if (!BuildViewport()) { return false; }
	if (!BuildShaders()) { return false; }
	if (!BuildInputLayout()) { return false; }
	if (!BuildVertexBuffer()) { return false; }
	if (!BuildConstantBuffers()) { return false; }
	return infoDump();
}

bool Renderer::BuildSwapChain(wWindow window)
{
	/// Swapchain
	DXGI_MODE_DESC displayDesc = {};
	displayDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	displayDesc.Height = window.GetWindowHeight();
	displayDesc.Width = window.GetWindowWidth();
	displayDesc.RefreshRate.Numerator = 144; // Hertz
	displayDesc.RefreshRate.Denominator = 1;
	displayDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	displayDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Anti-aliasing
	DXGI_SAMPLE_DESC multiSamplingDesc = {};
	multiSamplingDesc.Count = 1;
	multiSamplingDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.SampleDesc = multiSamplingDesc;
	swapChainDesc.BufferDesc = displayDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = window.Data();
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Windowed = true;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel[] = {
		D3D_FEATURE_LEVEL_11_0
	};
	UINT flags = 0;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		featureLevel, sizeof(featureLevel) / sizeof(*featureLevel),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(),
		NULL, // Gives which of feature levels is supported
		m_dContext.GetAddressOf()
	);
	return infoDump();
}
bool Renderer::BuildRenderTargetView()
{
	/// Render Target View
	return false;
}
bool Renderer::BuildViewport()
{
	/// Viewport

	return false;
}
bool Renderer::BuildShaders()
{
	/// Shaders

	return false;
}
bool Renderer::BuildInputLayout()
{
	/// Inputlayout

	return false;
}
bool Renderer::BuildVertexBuffer()
{
	/// Vertex Buffer
	D3D11_BUFFER_DESC vdesc = {};
	vdesc.Usage = D3D11_USAGE_DYNAMIC;
	vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vdesc.ByteWidth;
	vdesc.StructureByteStride;
	vdesc.MiscFlags;

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = m_assets.GetVertexVector().data();

	hr = m_device->CreateBuffer(&vdesc, &subData, m_vertexBuffer.GetAddressOf());
	if (FAILED(hr))
		return infoDump();
}
bool Renderer::BuildConstantBuffers()
{
	/// Constant buffers (material, light)

	return false;
}

// Returns true if no error is found
bool Renderer::infoDump()
{
	if (HRESULT(hr) == 0)
	{ return true; }


	return HRESULT(hr);
}






//Obj Parser
bool Renderer::ParseObj(std::vector<std::string> pathFiles)
{
	/// Parses the files included as parameter
	/// !OBS: The first input in the array/vector should always be equal to the 
	/// path of the files and empty if files are located in the same folder.
	for (int i = 1; i < pathFiles.size(); i++)
	{
		if (!m_assets.ParseFromObjFile(pathFiles[0], pathFiles[i], false))
			return false;
	}

	UpdateVertexBuffer();
	UpdateIndiceBuffer();
	return true;
}

//Renderer
void Renderer::Draw()
	// Everything to be rendered each frame
{
	ClearBuffer();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	for (auto& mapObj : m_assets.GetMeshMap())
	{
		


		//IA
		m_dContext->IASetVertexBuffers();
		m_dContext->IASetIndexBuffer();
		m_dContext->IASetInputLayout();
		m_dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_dContext->IASetIndexBuffer();

		//VS


		//RS
		m_dContext->RSSetViewports();

		//PS

		//OM
		m_dContext->OMSetRenderTargets();

		//Drawcall
		m_dContext->DrawIndexed(
			mapObj.second.GetIndiceCount(),
			mapObj.second.GetIndiceStartIndex(),
			mapObj.second.GetVerticeStartIndex()
		);

	}
	//End Frame
	m_swapChain->Present(1,0);
}

void Renderer::ClearBuffer()
{
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0 };
	m_dContext->ClearRenderTargetView(m_rtv.Get(), bgColor);
}
bool Renderer::UpdateVertexBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mResource;
	// Take pointer from GPU
	hr = m_dContext->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);

	// Penalty of reading what pData is pointing to
	memcpy(mResource.pData, 
		m_assets.GetVertexVector().data(), 
		m_assets.GetVertexVector().size() * sizeof(Vertex) 
	);

	// Give pointer back to GPU
	m_dContext->Unmap(m_vertexBuffer.Get(), 0); 

	return HRESULT(hr);
}
bool Renderer::UpdateIndiceBuffer()
{
	//TODO
	return HRESULT(hr);
}
