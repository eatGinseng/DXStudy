#include "cpuclass.h"


CpuClass::CpuClass()
{
}


CpuClass::CpuClass(const CpuClass& other)
{
}


CpuClass::~CpuClass()
{
}

// �ý����� ��� cpu usage�� �����ش�. ���� ���� ���� percentage�� 0���� �����Ѵ�.
// 1�ʿ� �� ���� üũ�ϵ��� Ÿ�̸Ӹ� ������ ���̴�. 
void CpuClass::Initialize()
{
	PDH_STATUS status;

	// system cpu usage�� ���� �� �ִ��� �ƴ���?
	m_canReadCpu = true;

	// cpu usage�� ���� quary object ����, 
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	// quary objet�� �ý����� ��� cou�� �����ϰԲ� ����
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_canReadCpu = false;
	}

	m_lastSampleTime = GetTickCount();

	m_cpuUsage = 0;

	return;
}

void CpuClass::Shutdown()
{
	if (m_canReadCpu)
	{
		PdhCloseQuery(m_queryHandle);
	}

	return;
}

void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_canReadCpu)
	{
		if ((m_lastSampleTime + 1000) < GetTickCount64())
		{
			m_lastSampleTime = GetTickCount64();

			PdhCollectQueryData(m_queryHandle);

			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &value);

			m_cpuUsage = value.longValue;
		}
	}

	return;
}

int CpuClass::GetCpuPercentage()
{
	int usage;

	if (m_canReadCpu)
	{
		usage = (int)m_cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}

