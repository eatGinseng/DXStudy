#include "PointLightClass.h"

PointLightClass::PointLightClass()
{
}


PointLightClass::PointLightClass(const PointLightClass& other)
{
}


PointLightClass::~PointLightClass()
{
}

void PointLightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMVectorSet(red, green, blue, alpha);
	return;
}


void PointLightClass::SetPosition(float x, float y, float z)
{
	m_position = XMVectorSet(x, y, z, 1.0f);
	return;
}

XMVECTOR PointLightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMVECTOR PointLightClass::GetPosition()
{
	return m_position;
}


