#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMVectorSet(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMVectorSet(red, green, blue, alpha);
	return;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}

// light�� �ٶ󺸴� ����, ���Ŀ� light�� view matrix�� ����� �� ���ȴ�.
void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
	return;

}

XMVECTOR LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

XMVECTOR LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT3 LightClass::GetPosition()
{
	return m_position;
}

XMVECTOR LightClass::GetDirection()
{
	XMVECTOR lookAt, pos;
	lookAt = XMLoadFloat3(&m_lookAt);
	pos = XMLoadFloat3(&m_position);

	XMVECTOR direction = XMVectorSubtract(lookAt, pos);
	direction = XMVector3Normalize(direction);

	return direction;
}