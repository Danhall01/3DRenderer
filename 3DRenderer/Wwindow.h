#pragma once

class Wwindow
{
public:


private:


private:


};

#include <Windows.h>

//Aka WindowsWindow Helper/Manager
class wWindow
{
public:
	wWindow() = delete;
	wWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow);


private:
	bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow);


public:
	HWND window;
	MSG msg = {};
};

