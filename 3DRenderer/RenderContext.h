#pragma once
#include "wWindow.h"
#include "Camera.h"




class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	wWindow CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction);
	void CreateDXCam();
	void CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void CreateDXCam(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const Camera& GetDXCamera() const;
	void AddDXCamPos(float x, float y, float z);

private:

private:
	Camera m_dxCam;
};