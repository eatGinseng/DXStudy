#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


#include <DirectXMath.h>
using namespace DirectX;
using namespace std;

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();
	
	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	XMVECTOR GetAmbientColor();
	XMVECTOR GetDiffuseColor();
	XMFLOAT3 GetDirection();
	XMVECTOR GetSpecularColor();
	float GetSpecularPower();

private:
	XMVECTOR m_ambientColor;
	XMVECTOR m_diffuseColor;
	XMFLOAT3 m_direction;
	XMVECTOR m_specularColor;
	float m_specularPower;
};

#endif
