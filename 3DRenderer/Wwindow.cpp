#include "Wwindow.h"
//Requirements
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Event inputs
	switch (message)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;


		//Key input handler
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		break;

	case WM_KEYUP:
		break;


	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

//The class
wWindow::wWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow)
{
	if (!SetupWindow(instance, width, height, nCmdShow))
	{
		std::cerr << "Failed to create Window" << std::endl;
		exit(1);
	}
}

bool wWindow::SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Test Window Class";
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;
	wc.style = CS_OWNDC;


	RegisterClass(&wc);
	window = CreateWindowEx(0, CLASS_NAME, L"Direct3D 11 Triangle", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);
	if (window == nullptr)
	{
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		return false;
	}

	ShowWindow(window, nCmdShow);
	return true;
}
