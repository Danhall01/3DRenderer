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

/// =============== Could be a class for input ===================
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
/// ==============================================================

// Funny
#define RANDOM_CUBES 1

int APIENTRY wWinMain(
	_In_     HINSTANCE   hInstance,
	_In_opt_ HINSTANCE   hPrevInstance,
	_In_     LPWSTR      lpCmdLine,
	_In_     int         nCmdShow)
{
#if RANDOM_CUBES
	// REMOVE ===================
	srand(1234567890);
	// ==========================
#endif
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Renderer renderer;
	wWindow hWindow = renderer.CreateWWindow(hInstance, nCmdShow, keyEvents);
	if (!renderer.Build(hWindow))
		return 1;

	std::array<float, 2> cursorPos = {};
	std::vector<std::string> inFiles = {
		"../External Resources/SceneObjects/",
		"cube.obj",
		"monkey.obj",
		"cubeLand.obj",
		"capsule.obj",
		"cubeMirror.obj"
	};
	if (!renderer.ParseObj(inFiles, 0))
		return 1;




	//########### SCENE ################
	
	// Create the render list
	std::vector<std::pair<std::string, DirectX::XMMATRIX>> drawable;
	
	std::string monkey  = "Suzanne";
	std::string floor   = "cubeLand";
	std::string cube    = "cube";
	std::string capsule = "capsule";

	//Mesh 1
	dx::XMMATRIX floorMatrix = dx::XMMatrixIdentity();
	floorMatrix *=
		dx::XMMatrixScaling(45, 1, 45) *
		dx::XMMatrixTranslation(0, -5, 0);
	drawable.push_back({ floor, floorMatrix });

	// Mesh 2
	dx::XMMATRIX cubeMatrix = dx::XMMatrixTranslation(5, 1, 0);
	drawable.push_back({ cube, cubeMatrix });

	// Mesh 3
	dx::XMMATRIX matrixCapsule = dx::XMMatrixIdentity();
	//matrixCapsule *= dx::XMMatrixRotationY(60);
	matrixCapsule *= dx::XMMatrixTranslation(0, 0, 5);
	drawable.push_back({ capsule, matrixCapsule });


#if RANDOM_CUBES
	// Spawn a random cube
	do 
	{
		dx::XMMATRIX spawnableMatrix = dx::XMMatrixIdentity();
		spawnableMatrix *= 
			dx::XMMatrixScaling(2.0f, 2.0f, 2.0f) *
			dx::XMMatrixTranslation(
				static_cast<float>(rand() % 300 -150),
				static_cast<float>(rand() % 300 -150),
				static_cast<float>(rand() % 300 -150));
		drawable.push_back({ cube, spawnableMatrix });
	} while ((drawable.size() < 100));
#endif

	// ########## Dynamic Cubic Environment Mapping ##########

	std::string DCEMShowcase = "mirror";
	dx::XMMATRIX DCEMMatrix = dx::XMMatrixIdentity();
	DCEMMatrix *= dx::XMMatrixTranslation(0, 0, 0);


	renderer.InitDCEM(DCEMShowcase, 400, 400);
	drawable.push_back({ DCEMShowcase, DCEMMatrix });
	
	// ########## Lighting ##########
	float shadowIndex = 0;

	Light lightTest = {};
	lightTest.Position_Type   = { 4.0f, 5.0f, 0.0f, LIGHT_TYPE_SPOTLIGHT };
	lightTest.Color_Intensity = { 0.7275f, 0.1412f, 0.2314f, 0.5f };
	lightTest.Direction_Range = { 0.0f, -1.0f, 0.0f, 50.0f};
	lightTest.CosOuter_Inner_SMap_count = { 0.7071f, 0.92f, 1.0f, shadowIndex++ };
	renderer.AddShadowLight(lightTest, hWindow);

	dx::XMMATRIX sunMatrix = dx::XMMatrixScaling(0.1f, 0.1f, 0.1f) *
		dx::XMMatrixTranslation(lightTest.Position_Type[0], lightTest.Position_Type[1] + 1, lightTest.Position_Type[2]);
	drawable.push_back({ monkey, sunMatrix });



	Light lightTest2 = {};
	lightTest2.Position_Type = { 15.0f, 5.0f, 0.0f, LIGHT_TYPE_SPOTLIGHT };
	lightTest2.Color_Intensity = { 0.3f, 0.8f, 0.2f, 1.5f };
	lightTest2.Direction_Range = { 0.0f, -1.0f, 0.0f, 20.0f };
	lightTest2.CosOuter_Inner_SMap_count = { 0.7071f, 0.92f, 1.0f, shadowIndex++ };
	renderer.AddShadowLight(lightTest2, hWindow);

	dx::XMMATRIX sunMatrix2 = dx::XMMatrixScaling(0.1f, 0.1f, 0.1f) *
		dx::XMMatrixTranslation(lightTest2.Position_Type[0], lightTest2.Position_Type[1] + 1, lightTest2.Position_Type[2]);
	drawable.push_back({ cube, sunMatrix2 });


	Light lightTest3 = {};
	lightTest3.Position_Type = { 0.0f, 15.0f, 0.0f, LIGHT_TYPE_DIRECTIONAL };
	lightTest3.Color_Intensity = { 1.0f, 1.0f, 1.0f, 0.5f };
	lightTest3.Direction_Range = { 0.0f, -1.0f, 0.0f, 20.0f };
	lightTest3.CosOuter_Inner_SMap_count = { 0.0f, 0.0f, 1.0f, shadowIndex++ };
	renderer.AddShadowLight(lightTest3, hWindow);

	dx::XMMATRIX sunMatrix3 = dx::XMMatrixScaling(0.1f, 0.1f, 0.1f) *
		dx::XMMatrixTranslation(lightTest3.Position_Type[0], lightTest3.Position_Type[1] + 1, lightTest3.Position_Type[2]);
	drawable.push_back({ cube, sunMatrix3 });


	Light lightTest4 = {};
	lightTest4.Position_Type = { -15.0f, 5.0f, 0.0f, LIGHT_TYPE_SPOTLIGHT };
	lightTest4.Color_Intensity = { 0.8275f, 0.8412f, 0.8314f, 1.5f };
	lightTest4.Direction_Range = { 0.0f, -1.0f, 0.0f, 50.0f };
	lightTest4.CosOuter_Inner_SMap_count = { 0.7071f, 0.92f, 1.0f, shadowIndex++ };
	renderer.AddShadowLight(lightTest4, hWindow);

	dx::XMMATRIX sunMatrix4 = dx::XMMatrixScaling(0.1f, 0.1f, 0.1f) *
		dx::XMMatrixTranslation(lightTest4.Position_Type[0], lightTest4.Position_Type[1] -5, lightTest4.Position_Type[2]);
	drawable.push_back({ cube, sunMatrix4 });
	// ############ Frustum Culling ############

#if FRUSTUM_CULLING
	renderer.InitFrustumCulling(drawable, 155.0f, -155.0f, 300);
#endif
	// Moving meshes (ignored by frustum culling)
	std::vector < std::pair<std::string, DirectX::XMMATRIX>> movable = {};
	// Moving Mesh 1
	dx::XMMATRIX matrix = dx::XMMatrixIdentity();
	movable.push_back({ monkey, matrix });


	// ############ Particles ############

	std::vector<ParticleData> particles;


	particles.push_back({ {-5.0f, 0.0f, 4.0f} });
	particles.push_back({ {-5.0f, 1.0f, 3.0f} });
	particles.push_back({ {-5.0f, 2.0f, 2.0f} });
	particles.push_back({ {-5.0f, 3.0f, 1.0f} });
	particles.push_back({ {-5.0f, 4.0f, 0.0f} });
	particles.push_back({ {-5.0f, 0.0f, -1.0f} });
	particles.push_back({ {-5.0f, 1.0f, -2.0f} });
	particles.push_back({ {-5.0f, 2.0f, -3.0f} });
	particles.push_back({ {-5.0f, 3.0f, -4.0f} });



	if (!renderer.InitParticles(particles, hWindow))
		return 1;

	//########### SCENE END ################


	using clock = std::chrono::high_resolution_clock;
	typedef std::chrono::duration<float> deltaTime;
	float moveDistance = 0.0f;

	auto startTime = clock::now();
	auto endTime = clock::now();
	while (hWindow.EventManager() != WM_QUIT)
	{
		renderer.UpdateDXCam();
		deltaTime frameTime = endTime - startTime;
		startTime = clock::now();

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
					0,
					0.004f
				);
			}
		}
		moveDistance += ( ((dx::XM_PI / 180) * 12.5f) * frameTime.count() );
		movable[0].second = dx::XMMatrixRotationY(dx::XM_PI / 180 * 90)
			* dx::XMMatrixTranslation(-15, 0, 0)
			* dx::XMMatrixRotationY(moveDistance);
		// ==========================
		


		// #################### Render loop ##########################


		renderer.Render(drawable, movable, hWindow);

			
		// #################### Render loop ##########################
		endTime = clock::now();
			
	}
	return 0;
}