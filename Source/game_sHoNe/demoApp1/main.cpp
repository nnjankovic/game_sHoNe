#include"GameTimer.h"
#include<thread>
#include<chrono>
#include<iostream>

#include "InitD3D.h"
#include "BoxApp.h"

using namespace std;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int nCmdShow)
{

	BoxApp app(hInstance);

	app.Initialize();

	app.Run();

	return 1;
}