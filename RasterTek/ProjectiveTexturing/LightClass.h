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
	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);

	XMVECTOR GetAmbientColor();
	XMVECTOR GetDiffuseColor();
	XMFLOAT3 GetPosition();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);

private:
	XMVECTOR m_ambientColor;
	XMVECTOR m_diffuseColor;
	XMFLOAT3 m_position;
	XMFLOAT3 m_lookAt;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;

};

#endif