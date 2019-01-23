#include"GameTimer.h"
#include<thread>
#include<chrono>
#include<iostream>
using namespace std;

int main()
{
	GameTimer timer;

	timer.Tick();

	this_thread::sleep_for(std::chrono::seconds(2));

	timer.Tick();

	cout << timer.DeltaTime() << endl;

	return 1;
}