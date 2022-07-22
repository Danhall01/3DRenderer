#include "Wwindow.h"
#include "Settings.h"

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
wWindow::wWindow(LPCWSTR windowName, HINSTANCE instance, float width, float height, 
	int nCmdShow, std::function<void(MSG&)> EventFunction)
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
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;


	RegisterClass(&wc);
	m_window = CreateWindowEx(0, CLASS_NAME, windowName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, (int)m_windowWidth, (int)m_windowHeight, nullptr, nullptr, instance, nullptr);
	if (m_window == nullptr)
	{
		exit(1);
	}
	ShowCursor(false);

	ShowWindow(m_window, nCmdShow);
}
wWindow::~wWindow(){}

const HWND& wWindow::Data() const { return m_window; }
const float wWindow::GetWindowHeight() const { return m_windowHeight; }
const float wWindow::GetWindowWidth() const { return m_windowWidth; }
const float wWindow::GetWindowRatio() const { return (float)(m_windowWidth) / (float)(m_windowHeight); }

#ifdef ARRAY
const bool wWindow::GetCursorPosition(float* data, const UINT& size) const
{
	if (size >= 2)
	{
		POINT cursorPos = {};
		GetCursorPos(&cursorPos);
		data[0] = static_cast<float>(cursorPos.x) - BASE_WINDOW_WIDTH / 2.0f;
		data[1] = static_cast<float>(cursorPos.y) - static_cast<int>(BASE_WINDOW_HEIGHT / 2.0f);
		SetCursorPos(static_cast<int>(BASE_WINDOW_WIDTH / 2.0f), static_cast<int>(BASE_WINDOW_HEIGHT / 2.0f));
		return true;
	}
	return false;
}
#endif
