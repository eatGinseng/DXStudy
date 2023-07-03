#include "FpsClass.h"


FpsClass::FpsClass()
{
}


FpsClass::FpsClass(const FpsClass& other)
{
}


FpsClass::~FpsClass()
{
}

// counter들을 0으로 초기화하고, 타이머를 시작한다.
void FpsClass::Initialize()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}


// 프레임 카운트를 1씩 증가시킨다.1초가 지났다면, 프레임 카운트를 fps에 저장한다. 그리고 카운트를 리셋하고, 타이머를 재시작한다.
void FpsClass::Frame()
{
	m_count++;
	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

// 방금 지난 1초의 FPS를 돌려준다.

int FpsClass::GetFps()
{
	return m_fps;
}
