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
	wWindow hWindow(L"Renderer", instance, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, nCmdShow, eventFunction);
	return hWindow;
}
