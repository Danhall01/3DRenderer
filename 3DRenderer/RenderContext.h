#pragma once
#include "wWindow.h"
#include "Camera.h"




class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	wWindow CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction);
	Camera CreateDXCam();
	Camera CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ);
	Camera CreateDXCam(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ);



private:
	

private:
	//std::vector<Model> m_modelVec;
};