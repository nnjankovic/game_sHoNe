#include "Scene.h"
#include "UserControllerListener.h"

using namespace std;

void CalculateFrameStats(shared_ptr<GameTimer> timer, Scene& scene)
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((timer->GameTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText =
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		scene.setFpsOnWindow(windowText);

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int nCmdShow)
{	
	auto gameTimer = make_shared<GameTimer>();
	gameTimer->Reset();


	auto inputListener = make_shared<UserControllerListener>(hInstance);
	inputListener->Start();

	Scene scene(hInstance, inputListener, gameTimer);
	scene.Init();

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gameTimer->Tick();
			CalculateFrameStats(gameTimer, scene);
			scene.Draw();
		}
	}

	return 1;
}