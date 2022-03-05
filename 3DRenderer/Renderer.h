#pragma once
#include "wWindow.h"
#include "Camera.h"
#include "Model.h"

#include <vector>

#define FREE(D3D11ptr) if(D3D11ptr != nullptr) { D3D11ptr->Release(); }


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

	void DXCleanUp();

	// ===== RENDERING FUNCTIONS
	Mesh& AddMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	const Mesh& GetMesh(int index) const;


	void DefaultPipeline();
	void SkeletonPipeline();

private:

private:
	Camera m_dxCam;
	std::vector<Mesh> m_models;
};