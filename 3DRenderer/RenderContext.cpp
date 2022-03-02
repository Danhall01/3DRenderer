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


wWindow RenderContext::CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction)
{
	return wWindow(L"Renderer", instance, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, nCmdShow, eventFunction);
}

Camera RenderContext::CreateDXCam()
{
	//Some default settings
	return Camera(0, 0, 0, 90, BASE_WINDOW_RATIO, 0.1, 100);
}
Camera RenderContext::CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	return Camera(pos, fovDegrees, aspectRatio, nearZ, farZ);
}
Camera RenderContext::CreateDXCam(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	return Camera(x, y, z, fovDegrees, aspectRatio, nearZ, farZ);
}
