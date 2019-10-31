#include "GameTimer.h"

#include <Windows.h>


GameTimer::GameTimer() :
	m_timerFrequency(0),
	m_deltaTime(0),
	m_prevTime(0),
	m_isPaused(false),
	m_lastPauseTime(0),
	m_totalPauseTime(0),
	m_currentTime(0)
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_timerFrequency);
}

double GameTimer::DeltaTime() const{
	std::lock_guard<std::mutex> lock(mutex);
	return m_deltaTime;
}

double GameTimer::GameTime() const {
	std::lock_guard<std::mutex> lock(mutex);

	auto endTime = m_isPaused ? m_lastPauseTime : m_currentTime;

	return (endTime - m_startTime) / m_timerFrequency - m_totalPauseTime / m_timerFrequency;
}

void GameTimer::Tick(){
	std::lock_guard<std::mutex> lock(mutex);

	double currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_deltaTime = (currentTime - m_prevTime) / m_timerFrequency;

	m_prevTime = currentTime;
	m_currentTime = currentTime;

	if (m_deltaTime < 0)
		m_deltaTime = 0;
}

void GameTimer::Reset() {
	std::lock_guard<std::mutex> lock(mutex);

	double currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_startTime = currentTime;
	m_prevTime = currentTime;
	m_totalPauseTime = 0;
	m_isPaused = false;
	m_lastPauseTime = 0;
	m_currentTime = 0;
}

void GameTimer::Pause() {
	std::lock_guard<std::mutex> lock(mutex);

	if (!m_isPaused)
	{
		double currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		m_lastPauseTime = currentTime;
		m_isPaused = true;
	}
}

void GameTimer::UnPause() {
	std::lock_guard<std::mutex> lock(mutex);

	if (m_isPaused)
	{
		double currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		m_totalPauseTime += currentTime - m_lastPauseTime;
		m_isPaused = false;
	}
}


 