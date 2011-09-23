#include "pch.h"
#include "GameTimer.h"

GameTimer::GameTimer()
	: m_DeltaTime(-1.0f)
	, m_BaseTime(0)
	, m_PausedTime(0)
	, m_PreviousTime(0)
	, m_StopTime(0)
	, m_IsStopped(false)
{
	long long countsPerSecond;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSecond));
	m_SecondsPerCount = 1.0f / (float)countsPerSecond;
}

float GameTimer::GetGameTime() const
{
	if (m_IsStopped)
	{
		return (m_StopTime - m_BaseTime - m_PausedTime) * m_SecondsPerCount;
	}
	else
	{
		long long currentTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

		return (currentTime - m_BaseTime - m_PausedTime) * m_SecondsPerCount;
	}
}

float GameTimer::GetDeltaTime() const
{
	return m_DeltaTime;
}

void GameTimer::Reset()
{
	long long currentTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

	m_BaseTime		= currentTime;
	m_PreviousTime	= currentTime;
	m_DeltaTime		= -1.0;
	m_PausedTime	= 0;
	m_StopTime		= 0;
	m_IsStopped		= false;
}

void GameTimer::Start()
{
	long long currentTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

	if (m_IsStopped)
	{
		m_PausedTime += (currentTime - m_StopTime);
		m_PreviousTime = currentTime;
		m_StopTime = 0;
		m_IsStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!m_IsStopped)
	{
		long long currentTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

		m_StopTime	= currentTime;
		m_IsStopped	= true;
	}
}

void GameTimer::Tick()
{
	if (m_IsStopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	long long currentTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentTime));

	m_DeltaTime = (currentTime - m_PreviousTime) * m_SecondsPerCount;
	m_PreviousTime = currentTime;

	if (m_DeltaTime < 0)
	{
		m_DeltaTime = 0.0;
	}
}
