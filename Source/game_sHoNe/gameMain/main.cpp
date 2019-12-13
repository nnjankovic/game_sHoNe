#include "Scene.h"
#include "UserControllerListener.h"

using namespace std;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int nCmdShow)
{	
	auto gameTimer = make_shared<GameTimer>();

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
			scene.Draw();
		}
	}

	return 1;
}