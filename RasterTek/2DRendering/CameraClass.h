#pragma once

#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <DirectXMath.h>
using namespace DirectX;

class CameraClass {
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	// 카메라 위치와 회전값을 지정
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	// view matrix를 만드는 데 카메라 위치와 회전 정보를 활용할 것임
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;

};

#endif