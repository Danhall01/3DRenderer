#include <Windows.h>
#include <atomic>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timeLock(16ms);

void End(int state)
{
	//Do cleanup

	exit(state);
}


//Thread Data
std::atomic_bool exitThread = false;

//Shared event data
std::atomic_bool quit = false;

void EventManager() 
{
	while (!exitThread)
	{
		//Windows API

	}
	return;
}


int APIENTRY wWinMain(
	_In_     HINSTANCE   hInstance,
	_In_opt_ HINSTANCE   hPrevInstance,
	_In_     LPWSTR      lpCmdLine,
	_In_     int         nCmdShow)
{
	using clock = std::chrono::high_resolution_clock;
	std::chrono::nanoseconds elapsedTime(0ns);
	auto startTime = clock::now();

	//Create Thread
	std::thread handle(EventManager);

	
	while (!quit)
	{
		auto DTime = clock::now() - startTime;
		startTime = clock::now();
		elapsedTime += std::chrono::duration_cast<std::chrono::nanoseconds>(DTime);


		//Tick based system
		while (elapsedTime >= timeLock)
		{
			//Update game logic


			//Catch up the loop
			elapsedTime -= timeLock;
		}

		//Do D3D11 stuff
		//Send the shared data from the event handler as input

		

	}
	//Cleanup threadwork
	exitThread = true;
	handle.join();


	return 0;
}