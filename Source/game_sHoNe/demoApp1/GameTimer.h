#pragma once
#if !defined _GAMETIMER_H_
#define _GAMETIMER_H_

#include <mutex>

class GameTimer
{
public:
	GameTimer();

	double DeltaTime() const; //in seconds
	double GameTime() const; //in seconds

	void Reset(); // Call before message loop.
	void Pause(); // Call when unpaused.
	void UnPause();  // Call when paused.
	void Tick();  // Call every frame.

private:
	double m_timerFrequency; // ticks per second
	
	/* time is measured in ticks */
	double m_deltaTime;
	double m_prevTime;
	double m_startTime;
	double m_totalPauseTime;
	double m_lastPauseTime;
	double m_currentTime;

	bool m_isPaused;

private:
	mutable std::mutex mutex;
};

#endif
