#include <Windows.h>

#include "Renderer.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

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

	case VK_LCONTROL: //LCTRL
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
		// Get normalized vector from mouse position
		// Reset mouse position to center
		// Move camera in that direction

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


	using clock = std::chrono::high_resolution_clock;
	std::chrono::nanoseconds elapsedTime(0ns);
	auto startTime = clock::now();

	Renderer renderer;
	wWindow hWindow = renderer.CreateWWindow(hInstance, nCmdShow, keyEvents);
	

	//Hardcoded test
	std::vector<Vertex> vList;
	vList.push_back({ {-1.0f,  1.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }); //Top left
	vList.push_back({ { 1.0f,  1.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }); //Top right
	vList.push_back({ {-1.0f, -1.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }); //Bottom left
	vList.push_back({ { 1.0f, -1.0f, 0}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }); //Bottom right
	std::vector<unsigned int> indices = {
	0, 1, 3,
	0, 3, 2
	};

	//This will mostly happen internally with the readObj function
	Mesh& m = renderer.AddMesh(vList, indices);
	m.AddSubMesh(0,6);


	float speedMultiplier = 0.43f;
	while (hWindow.EventManager() != WM_QUIT)
	{
		auto DTime = clock::now() - startTime;
		startTime = clock::now();
		elapsedTime += std::chrono::duration_cast<std::chrono::nanoseconds>(DTime);

		//Time based system
		while (elapsedTime >= timeLock)
		{
			//Update game logic
			renderer.AddDXCamPos( //Move the camera
				speedMultiplier * right - speedMultiplier * left,
				speedMultiplier * up - speedMultiplier * down,
				speedMultiplier * forward - speedMultiplier * backward
			);
			
			//Mesh => Verticies
			//		Indicies
			//		Pipeline Selection

			//Submesh => startIndex (From the indicies)
			//			Amount of Indicies used
			//			TextureNR


			//Catch up the loop
			elapsedTime -= timeLock;
		}
	}
	return 0;
}