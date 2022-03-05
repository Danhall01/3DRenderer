#include "Wwindow.h"


//The event handlers
UINT wWindow::EventManager()
{
	MSG msg = {};
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		if (IsDialogMessage(m_window, &msg))
		{
			m_eventFunction(msg);
		}
	}
	return msg.message;
}
void DefaultEvent(MSG& msg)
{
	//Event inputs
	switch (msg.message)
	{
	//Key input handler
	case WM_KEYDOWN:
		switch (msg.wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return;
		}
		break;

	case WM_KEYUP:
		break;


	default:
		break;
	}
	return;
}
LRESULT CALLBACK defaultWndProc(HWND aHWnd, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
	switch (aMessage)
	{
	case WM_DESTROY: //Cross
	case WM_CLOSE: //Close window
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(aHWnd, aMessage, aWParam, aLParam);
	}
	return 0;
}


//The class
wWindow::wWindow(LPCWSTR windowName, HINSTANCE instance, UINT width, UINT height, int nCmdShow, std::function<void(MSG&)> EventFunction)
{
	//init variables
	m_windowWidth = width;
	m_windowHeight = height;
	if (EventFunction == nullptr) 
	{
		m_eventFunction = DefaultEvent;
	}
	else
	{
		m_eventFunction = EventFunction;
	}

	//Init window
	const wchar_t CLASS_NAME[] = L"Window Class";
	WNDCLASS wc = { };

	wc.lpfnWndProc = defaultWndProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;
	wc.style = CS_OWNDC;


	RegisterClass(&wc);
	m_window = CreateWindowEx(0, CLASS_NAME, windowName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, m_windowWidth, m_windowHeight, nullptr, nullptr, instance, nullptr);
	if (m_window == nullptr)
	{
		exit(1);
	}

	ShowWindow(m_window, nCmdShow);
}
wWindow::~wWindow(){}

const HWND& wWindow::GetData() const { return m_window; }
const UINT wWindow::GetWindowHeight() const { return m_windowHeight; }
const UINT wWindow::GetWindowWidth() const { return m_windowWidth; }
const float wWindow::GetWindowRatio() {	return (float)(m_windowWidth) / (float)(m_windowHeight); }