#include"GameTimer.h"
#include<thread>
#include<chrono>
#include<iostream>

#include "InitD3D.h"

using namespace std;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int nCmdShow)
{

	InitD3D app(hInstance);

	app.Initialize();

	app.Run();

	return 1;
}