#ifndef WWINDOW_HPP
#define WWINDOW_HPP
#include <Windows.h>
#include <functional>


class wWindow
{
public:
	wWindow() = delete;
	wWindow(LPCWSTR windowName, HINSTANCE instance, UINT width, UINT height, int nCmdShow, std::function<void(MSG&)> EventFunction);
	~wWindow();

	const HWND& GetData() const;

	const UINT GetWindowHeight() const;
	const UINT GetWindowWidth() const;
	const float GetWindowRatio();


	UINT EventManager();
private:
	

private:
	HWND m_window;

	//bool m_fullscreen;
	UINT m_windowHeight;
	UINT m_windowWidth;

	std::function<void(MSG&)> m_eventFunction;
};
#endif