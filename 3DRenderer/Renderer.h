#pragma once
//  Helper classes
#include "wWindow.h"
#include "Camera.h"
#include "Assets.h"
// The API
#include <d3d11.h>
// Other libraries
#include <vector>
#include <string>
#include <wrl/client.h>
using namespace Microsoft;


constexpr bool _Debug_ = 0;
class Renderer
{
public:
	Renderer();
	~Renderer();
	
	// ===== HELPER FUNCTIONS
#ifdef WWINDOW_HPP
	wWindow CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction);
#endif
	void CreateDXCam();
	void CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void CreateDXCam(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const Camera& GetDXCamera() const;
	void AddDXCamPos(float x, float y, float z);

	// bool Build(HWND window) ...
	bool Build(wWindow window);


	// ===== RENDERING FUNCTIONS
	//Input manager
	bool ParseObj(std::vector<std::string> pathFiles);

	//Renderer manager
	void Draw();
	void ClearBuffer();
private:
	// Hresult manager
	bool infoDump();

	// Rendering Helper Functions
	bool UpdateVertexBuffer();
	bool UpdateIndiceBuffer();

	// Build functions
	bool BuildSwapChain(wWindow window);
	bool BuildRenderTargetView();
	bool BuildViewport();
	bool BuildShaders();
	bool BuildInputLayout();
	bool BuildVertexBuffer();
	bool BuildConstantBuffers();

private:
	Camera m_dxCam;
	Assets m_assets;
	HRESULT hr;

private: //D3D11 VARIABLES
	WRL::ComPtr<ID3D11Device>           m_device;
	WRL::ComPtr<ID3D11DeviceContext>    m_dContext;

	WRL::ComPtr<IDXGISwapChain>         m_swapChain;
	WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
	WRL::ComPtr<ID3D11InputLayout>      m_inputLayout;
	WRL::ComPtr<ID3D11Buffer>           m_vertexBuffer;

	D3D11_VIEWPORT                      m_viewport;
	//Shaders
	WRL::ComPtr<ID3D11VertexShader>     m_vertexShader;
	WRL::ComPtr<ID3D11PixelShader>      m_pixelShader;
	WRL::ComPtr<ID3D11GeometryShader>   m_geometryShader;

	//Buffers
	WRL::ComPtr<ID3D11Buffer>           m_vertexCBuffer;

};
