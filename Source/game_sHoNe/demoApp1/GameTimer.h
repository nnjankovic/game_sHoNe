#pragma once
#if !defined _GAMETIMER_H_
#define _GAMETIMER_H_

class GameTimer
{
public:
	GameTimer();

	double deltaTimeSeconds();

	void Tick();

private:
	double m_timerFrequency;
	double m_deltaTime;
	double m_prevTime;
};

#endif
