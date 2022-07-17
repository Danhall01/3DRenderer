#pragma once
//  Helper classes
#include "wWindow.h"
#include "Camera.h"
#include "Assets.h"
// The API
#include <d3d11.h>
// Other libraries
#include <unordered_map>
#include <vector>
#include <string>
#include <wrl/client.h>
using namespace Microsoft;

#include "Structures.h"

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
	// bool BuildShaderAndLayout(wWindow window, UINT assetPack, unsigned char flag); // Flag identifies which layout and shader to use together

	// ===== RENDERING FUNCTIONS
	//Input manager
	bool ParseObj(std::vector<std::string> pathFiles, unsigned char flag);
	    // Returns true if mesh is found, else false
	bool SetMeshMatrix(std::string id,
		const dx::XMMATRIX& matrix
	);



	//Renderer manager
	void Draw(std::vector<std::string> drawTargets);
	void Draw(std::vector< std::pair<std::string, dx::XMMATRIX> > drawTargets);
	void ClearBuffer();
private:
	// Hresult manager
	inline void infoDump(unsigned line);


	// Build functions
	bool BuildSwapChain(wWindow window);
	bool BuildRenderTargetView();
	bool BuildViewport(wWindow window);
	bool BuildShadersDefault(ID3DBlob*& out_shaderBlob);
	bool BuildInputLayoutDefault(ID3DBlob*& relativeSBlob);

	// Index / Vertex buffer functions
	bool BuildVertexBuffer();
	bool BuildIndexBuffer();
	bool UpdateVertexBuffer(Assets& asset);
	bool UpdateIndexBuffer(Assets& asset);

	// Mesh Helper Functions
	bool BuildVertexConstantBuffer();
	bool UpdateVertexConstantBuffer(const Mesh& mesh);
	bool UpdateVertexConstantBuffer(dx::XMMATRIX& matrix);
private:
	HRESULT m_hr;
	Camera m_dxCam;
	// unsigned char: ob0000 for default, see README.md
	// Assets: The assets used for a specific flag
	std::unordered_map<unsigned char, Assets> m_assets;
	

private: //D3D11 VARIABLES
	WRL::ComPtr<ID3D11Device>                   m_device;
	WRL::ComPtr<ID3D11DeviceContext>            m_dContext;

	WRL::ComPtr<IDXGISwapChain>                 m_swapChain;
	WRL::ComPtr<ID3D11RenderTargetView>         m_rtv;
	WRL::ComPtr<ID3D11DepthStencilView>         m_dsv;

	WRL::ComPtr<ID3D11Buffer>                   m_indexBuffer;
	WRL::ComPtr<ID3D11Buffer>                   m_vertexBuffer;

	D3D11_VIEWPORT                              m_viewport;
	//Shaders
	std::vector<shaderSet>                      m_shaders;
	std::vector<WRL::ComPtr<ID3D11InputLayout>> m_inputLayout;

	// Tips and tricks
	// Source: https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0
	// 1. Do not swap between different constant buffers
	// 2. Constant buffer that gets updated should only affect one step in the pipeline
	// 3. Do not read from the Map[MAP_WRITE_DISCARD] data
	WRL::ComPtr<ID3D11Buffer>                   m_vConstBuffer;
};
