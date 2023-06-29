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

// counter���� 0���� �ʱ�ȭ�ϰ�, Ÿ�̸Ӹ� �����Ѵ�.
void FpsClass::Initialize()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}


// ������ ī��Ʈ�� 1�� ������Ų��.1�ʰ� �����ٸ�, ������ ī��Ʈ�� fps�� �����Ѵ�. �׸��� ī��Ʈ�� �����ϰ�, Ÿ�̸Ӹ� ������Ѵ�.
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

// ��� ���� 1���� FPS�� �����ش�.

int FpsClass::GetFps()
{
	return m_fps;
}
