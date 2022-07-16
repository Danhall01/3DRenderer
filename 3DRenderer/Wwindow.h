#ifndef WWINDOW_HPP
#define WWINDOW_HPP
#include <Windows.h>
#include <functional>


class wWindow
{
public:
	wWindow() = delete;
	wWindow(LPCWSTR windowName, HINSTANCE instance, float width, float height, int nCmdShow, std::function<void(MSG&)> EventFunction);
	~wWindow();

	const HWND& Data() const;

	const float GetWindowHeight() const;
	const float GetWindowWidth() const;
	const float GetWindowRatio();


	UINT EventManager();
private:
	

private:
	HWND m_window;

	//bool m_fullscreen;
	float m_windowHeight;
	float m_windowWidth;

	std::function<void(MSG&)> m_eventFunction;
};
#endif