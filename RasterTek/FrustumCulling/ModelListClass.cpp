#include "ModelListClass.h"

ModelListClass::ModelListClass()
{
	m_ModelInfoList = 0;
}


ModelListClass::ModelListClass(const ModelListClass& other)
{
}


ModelListClass::~ModelListClass()
{
}

bool ModelListClass::Initialize(int numModels)
{
	int i;
	float red, green, blue;

	// 사용될 모델의 갯수ㅇ를 저장하고, ModelInfoType 구조체로 리스트를 초기화한다.
	m_modelCount = numModels;

	// model 정보 배열을 만든다.
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	// 현재 시간으로 랜덤 generator에 seed를 준다.
	srand((unsigned int)time(NULL));

	for (i = 0; i < m_modelCount; i++)
	{
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = XMVectorSet(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;

	}

	return true;

}

void ModelListClass::Shutdown()
{
	// Release the model information list.
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}

	return;
}

int ModelListClass::GetModelCount()
{
	return m_modelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMVECTOR& color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;

	return;
}

