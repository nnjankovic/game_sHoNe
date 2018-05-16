#include "GameTimer.h"

#include <Windows.h>


GameTimer::GameTimer() :
	m_timerFrequency(0),
	m_deltaTime(0),
	m_prevTime(0)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_timerFrequency);
}

double GameTimer::deltaTimeSeconds()
{
	return m_deltaTime;
}

void GameTimer::Tick()
{
	double currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_deltaTime = (currentTime - m_prevTime) / m_timerFrequency;

	m_prevTime = currentTime;

	if (m_deltaTime < 0)
		m_deltaTime = 0;
}


 