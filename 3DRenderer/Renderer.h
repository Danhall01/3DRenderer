#pragma once
//  Helper classes
#include "wWindow.h"
#include "Camera.h"
#include "Assets.h"
#include "ShadowLight.h"
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
	void CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, 
		float aspectRatio, float nearZ, float farZ);
	void CreateDXCam(float x, float y, float z, float fovDegrees, 
		float aspectRatio, float nearZ, float farZ);

	const Camera& GetDXCamera() const;
	void AddDXCamPos(float right, float up, float forward);
	void RotateDXCam(float pitch, float yaw, float roll);
	void UpdateDXCam();

	void AddLight(const Light& light);
	void AddShadowLight(const Light& light, const wWindow& window);

	// ===== RENDERING FUNCTIONS
	//Input manager
	bool ParseObj(std::vector<std::string> pathFiles, unsigned char flag);
	    // Returns true if mesh is found, else false
	bool SetMeshMatrix(std::string id,
		const dx::XMMATRIX& matrix
	);

	//Renderer manager
	bool Build(wWindow window);
	void DrawDeferred(std::vector< std::pair<std::string, dx::XMMATRIX> >& drawTargets, const wWindow& window);
	void ShadowPass(std::vector< std::pair<std::string, dx::XMMATRIX> >& drawTargets);
	
	bool UpdateLighting();
	void Render();
private:
	// Renderer Helper Functions
	void ClearBuffer();
	void RenderDrawTargets(const Assets& currentAsset,
		const UINT& stride, const UINT& offset,
		std::vector< std::pair<std::string, dx::XMMATRIX> >& drawTargets);

	// Hresult manager
	void infoDump(unsigned line);


	// Build functions
	bool BuildDeviceAndSwapChain(const wWindow& window);
	bool BuildRenderTargetView();
	bool BuildViewport(const wWindow& window);
	// Build Shaders
	bool BuildShadersDeferred(ID3DBlob*& out_shaderBlob);
	bool BuildInputLayoutDeferred(ID3DBlob*& shaderBlob);
	//Build Deferred
	bool BuildGraphBuffer(const wWindow& window);
	bool BuildUnorderedAccessView(const wWindow& window);
	//Build LOD
	bool BuildLOD(ID3DBlob* shaderBlob);


	// Index / Vertex buffer functions
	bool BuildVertexBuffer();
	bool BuildIndexBuffer();
	bool UpdateVertexBuffer(Mesh& mesh);
	bool UpdateIndexBuffer(Mesh& mesh);


	// Mesh Helper Functions
	bool BuildVertexConstantBuffer();
	bool UpdateVertexConstantBuffer(const Mesh& mesh);
	bool UpdateVertexConstantBuffer(dx::XMMATRIX& matrix);
	bool UpdateVertexConstantBuffer(dx::XMMATRIX& modelView, const Camera& cam);


	// Texture Helper Functions
	bool BuildSampler();
	bool BuildTextureBuffer();
	bool UpdateTextureBuffer(const TextureData& texture);

	// Lights Helper Functions
	bool BuildLightBuffer();
	bool BuildShadowPass(ID3DBlob* shaderBlob, wWindow window);

	// LOD Helper Functions
	bool UpdateLODCBuffer(const Mesh& mesh, float tesFactor);


	// Image Helper functions
	bool UpdateImageMap();
private:
	HRESULT                                     m_hr;
	Camera                                      m_dxCam;
	// unsigned char: ob0000 for default, see README.md
	// Assets: The assets used for a specific flag
	std::unordered_map<unsigned char, Assets>   m_assets;
	

private: //D3D11 VARIABLES
	WRL::ComPtr<ID3D11Device>                   m_device;
	WRL::ComPtr<ID3D11DeviceContext>            m_immediateContext;
	WRL::ComPtr<IDXGISwapChain>                 m_swapChain;
	WRL::ComPtr<ID3D11RenderTargetView>         m_rtv;
	D3D11_VIEWPORT                              m_viewport;


	WRL::ComPtr<ID3D11Buffer>                   m_indexBuffer;
	WRL::ComPtr<ID3D11Buffer>                   m_vertexBuffer;
	WRL::ComPtr<ID3D11Buffer>                   m_vConstBuffer;
	WRL::ComPtr<ID3D11Buffer>                   m_materialBuffer;
	WRL::ComPtr<ID3D11SamplerState>             m_samplerState;
	std::unordered_map<
		std::string, 
		WRL::ComPtr<ID3D11ShaderResourceView>>  m_loadedImages;


	//Shaders
	std::vector<ShaderSet>                      m_shaders;
	std::vector<WRL::ComPtr<ID3D11InputLayout>> m_inputLayout;

	WRL::ComPtr<ID3D11Buffer>					m_LODCBuffer;
	WRL::ComPtr<ID3D11VertexShader>             m_shadowVertexShader;


	//Deferred rendering
	static constexpr UINT BUFFER_COUNT = 5; // Base 5 gbuffers, 1 lights, 1 shadows
	std::vector<WRL::ComPtr<ID3D11UnorderedAccessView>> m_uav; // swapchainBuffer
	GraphicsBuffer                                      m_gbuffer[BUFFER_COUNT];
	WRL::ComPtr<ID3D11DepthStencilView>                 m_dsv;
	
	// Ambient color : texture 2d
	// WS pos        : texture 2d
	// normal        : texture 2d
	// diffuse color : texture 2d
	// specular color: texture 2d
	WRL::ComPtr<ID3D11RenderTargetView>                 m_deferredRTVOutput[BUFFER_COUNT] = {};
	WRL::ComPtr<ID3D11ShaderResourceView>               m_deferredSRVInput[BUFFER_COUNT+1] = {}; // +1 = lights
	
																								 
																 // Lighting
	WRL::ComPtr<ID3D11Buffer>                           m_lightCount;
	WRL::ComPtr<ID3D11Buffer>                           m_lightBuffer;
	std::vector<Light>                                  m_lightArr;
	ShadowLight                                         m_shadowlightManager;

	// Tips and tricks (Constant buffer)
	// Source: https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0
	// 1. Do not swap between different constant buffers
	// 2. Constant buffer that gets updated should only affect one step in the pipeline
	// 3. Do not read from the Map[MAP_WRITE_DISCARD] data
};
