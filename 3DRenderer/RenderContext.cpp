#include "RenderContext.h"
#include "Settings.h"

//Maybe works
//wWindow& RenderContext::CreateWWindow(LPCWSTR windowName, HINSTANCE instance, UINT height, UINT width, int nCmdShow)
//{
//	std::function<void(MSG&)> func = std::bind(&RenderContext::windowEventManager, this, std::placeholders::_1);
//	wWindow hwindow(windowName, instance, width, height, nCmdShow, func);
//	return hwindow;
//}

RenderContext::RenderContext(){}
RenderContext::~RenderContext(){}

//Window helper
wWindow RenderContext::CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction)
{
	return wWindow(L"Renderer", instance, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, nCmdShow, eventFunction);
}


//Camera Manager
void RenderContext::CreateDXCam()
{
	//Some default settings
	m_dxCam = Camera(0, 0, 0, 90, BASE_WINDOW_RATIO, 0.1f, 100.0f);
}
void RenderContext::CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_dxCam = Camera(pos, fovDegrees, aspectRatio, nearZ, farZ);
}
void RenderContext::CreateDXCam(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_dxCam = Camera(x, y, z, fovDegrees, aspectRatio, nearZ, farZ);
}
const Camera& RenderContext::GetDXCamera() const { return m_dxCam; }
void RenderContext::AddDXCamPos(float x, float y, float z)
{
	m_dxCam.AddPosition(x, y, z);
}


