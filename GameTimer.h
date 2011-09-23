#pragma once

class GameTimer
{
public:
    GameTimer();

	float GetGameTime() const;	// in seconds
	float GetDeltaTime() const;	// in seconds

	void Reset();	// Call before message loop.
	void Start();	// Call when unpaused.
	void Stop();	// Call when paused.
	void Tick();	// Call every frame.

private:
	float m_SecondsPerCount;
	float m_DeltaTime;

	long long m_BaseTime;
	long long m_PausedTime;
	long long m_StopTime;
	long long m_PreviousTime;

	bool m_IsStopped;
};
