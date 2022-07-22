#include <Windows.h>

//D3D11
#include "Renderer.h"

//Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//Clock
#include <chrono>
using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timeLock(16ms);

//Gloabal variables for keybinds
static bool up;
static bool down;
static bool forward;
static bool backward;
static bool left;
static bool right;

//Keybind manager (Should come from the game engine)
void keyDown(WPARAM key)
{
	//Assume key is always something
	switch (key)
	{
	case VK_ESCAPE:
		PostQuitMessage(0);
		return;

	case 0x41: //A
		left = true;
		break;

	case 0x44: //D
		right = true;
		break;

	case 0x57: //W
		forward = true;
		break;

	case 0x53: //S
		backward = true;
		break;

	case VK_CONTROL: //CTRL
		down = true;
		break;

	case VK_SPACE: //Spacebar
		up = true;
		break;
	}
}
void keyUp(WPARAM key)
{
	//Assume key is always something
	switch (key)
	{
	case 0x41: //A
		left = false;
		break;

	case 0x44: //D
		right = false;
		break;

	case 0x57: //W
		forward = false;
		break;

	case 0x53: //S
		backward = false;
		break;

	case VK_CONTROL: //LCTRL
		down = false;
		break;

	case VK_SPACE: //Spacebar
		up = false;
		break;
	}
}
void keyEvents(MSG& msg)
{
	//Event inputs
	switch (msg.message)
	{
	// ========= KEYDOWN =========
	case WM_KEYDOWN:
		keyDown(msg.wParam);
		break;


		// ========= KEYUP =========
	case WM_KEYUP:
		keyUp(msg.wParam);
		break;

		// ========= MISC EVENTS =========


	default:
		break;
	}
	return;
}


int APIENTRY wWinMain(
	_In_     HINSTANCE   hInstance,
	_In_opt_ HINSTANCE   hPrevInstance,
	_In_     LPWSTR      lpCmdLine,
	_In_     int         nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Renderer renderer;
	wWindow hWindow = renderer.CreateWWindow(hInstance, nCmdShow, keyEvents);
	if (!renderer.Build(hWindow))
		return 1;

	std::array<float, 2> cursorPos = {};
	std::vector<std::string> inFiles = {
		"../External Resources/SceneObjects/",
		"Cube_triangulated.obj",
		"monkey.obj"
	};
	renderer.ParseObj(inFiles, 0);

	// Create the render list
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> drawable;
	
	std::string rect = "Cube_Cube.001";
	dx::XMMATRIX matrix = dx::XMMatrixIdentity();
	drawable.push_back({rect, matrix});

	std::string monkey = "Suzanne";
	dx::XMMATRIX matrixMonkey = dx::XMMatrixIdentity();
	matrixMonkey *= dx::XMMatrixRotationY(60);
	matrixMonkey *= dx::XMMatrixTranslation(0, 0, 5);
	drawable.push_back({ monkey, matrixMonkey });


	using clock = std::chrono::high_resolution_clock;
	std::chrono::nanoseconds elapsedTime(0ns);
	auto startTime = clock::now();
	while (hWindow.EventManager() != WM_QUIT)
	{
		auto DTime = clock::now() - startTime;
		startTime = clock::now();
		elapsedTime += std::chrono::duration_cast<std::chrono::nanoseconds>(DTime);

		//Time based system
		while (elapsedTime >= timeLock)
		{
			//Update game logic
			if (right || left || up || down || forward || backward)
			{
				renderer.AddDXCamPos( //Move the camera
					(float)right   - left,
					(float)up      - down,
					(float)forward - backward);
			}
			if (hWindow.Data() == GetForegroundWindow())
			{
				hWindow.GetCursorPosition(cursorPos.data(), static_cast<UINT>(cursorPos.size()));
				if (cursorPos[0] != 0 && cursorPos[0] < hWindow.GetWindowWidth() / 2 ||
					cursorPos[1] != 0 && cursorPos[1] < hWindow.GetWindowHeight() /2 )
				{
					renderer.RotateDXCam(
						cursorPos[1],
						cursorPos[0],
						0
					);
				}
			}
			renderer.UpdateDXCam();

			//Render
			renderer.Draw(drawable);

			//Catch up the loop
			elapsedTime -= timeLock;
		}
	}
	return 0;
}