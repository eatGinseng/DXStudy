#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


#include <DirectXMath.h>
using namespace DirectX;
using namespace std;

class PointLightClass
{
public:
	PointLightClass();
	PointLightClass(const PointLightClass&);
	~PointLightClass();
	

	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);

	XMVECTOR GetDiffuseColor();
	XMVECTOR GetPosition();

private:

	XMVECTOR m_diffuseColor;
	XMFLOAT3 m_position;

};

#endif
