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


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMVectorSet(red, green, blue, alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMVectorSet(x, y, z, 1.0f);
	return;
}


XMVECTOR LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMVECTOR LightClass::GetDirection()
{
	return m_direction;
}