#include "TimerClass.h"

TimerClass::TimerClass()
{

}

TimerClass::TimerClass(const TimerClass& other)
{

}

TimerClass::~TimerClass()
{

}

// 시스템에서 고정밀 타이머를 지원하는지 먼저 확인. frequency로 각 밀리초마다 Counter Tick이 몇 번 일어났는지 본다.
// frame time을 계산하는 데 그 값을 사용한다.
bool TimerClass::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;


	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;

	return;
}

float TimerClass::GetTime()
{
	return m_frameTime;
}
