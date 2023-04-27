#include "FrustumClass.h"

FrustumClass::FrustumClass()
{
}


FrustumClass::FrustumClass(const FrustumClass& other)
{
}


FrustumClass::~FrustumClass()
{
}

// screen의 depth와 positionMatrix, viewmatrix를 얻어 매 프레임 호출된다. 해당 프레임의 view frustum을 만든다.
// frustum의 plane 6개를 만든다.
void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMininum, r;
	XMMATRIX matrix;

	XMFLOAT4X4 projectionTmp;
	XMStoreFloat4x4(&projectionTmp, projectionMatrix);

	// frustum의 최소 z 거리를 계산한다.
	zMininum = -projectionTmp._43 / projectionTmp._33;
	r = screenDepth / (screenDepth - zMininum);
	projectionTmp._33 = r;
	projectionTmp._32 = -r * zMininum;

	projectionMatrix = XMLoadFloat4x4(&projectionTmp);

	// view matrix와 업데이트된 projection matrix로 frustum matrix를 만든다.
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// frustum의 near plane 생성
	XMFLOAT4X4 matrixTmp;
	XMStoreFloat4x4(&matrixTmp, matrix);

	m_Planes[0] = XMVectorSet(	matrixTmp._14 + matrixTmp._13,
								matrixTmp._24 + matrixTmp._23,
								matrixTmp._34 + matrixTmp._33,
								matrixTmp._44 + matrixTmp._43 );

	m_Planes[0] = XMPlaneNormalize(m_Planes[0]);

	// Far Plane 생성
	m_Planes[1] = XMVectorSet(	matrixTmp._14 - matrixTmp._13,
								matrixTmp._24 - matrixTmp._23,
								matrixTmp._34 - matrixTmp._33,
								matrixTmp._44 - matrixTmp._43);
	m_Planes[1] = XMPlaneNormalize(m_Planes[1]);

	// Left plane 생성
	m_Planes[2] = XMVectorSet(	matrixTmp._14 + matrixTmp._13,
								matrixTmp._24 + matrixTmp._23,
								matrixTmp._34 + matrixTmp._33,
								matrixTmp._44 + matrixTmp._43);
	m_Planes[2] = XMPlaneNormalize(m_Planes[2]);

	// Right Plane 생성
	m_Planes[3] = XMVectorSet(	matrixTmp._14 - matrixTmp._13,
								matrixTmp._24 - matrixTmp._23,
								matrixTmp._34 - matrixTmp._33,
								matrixTmp._44 - matrixTmp._43);
	m_Planes[3] = XMPlaneNormalize(m_Planes[3]);

	// top plane 생성
	m_Planes[4] = XMVectorSet(	matrixTmp._14 - matrixTmp._13,
								matrixTmp._24 - matrixTmp._23,
								matrixTmp._34 - matrixTmp._33,
								matrixTmp._44 - matrixTmp._43);
	m_Planes[4] = XMPlaneNormalize(m_Planes[4]);

	// Calculate bottom plane of frustum.
	m_Planes[5] = XMVectorSet(	matrixTmp._14 + matrixTmp._13,
								matrixTmp._24 + matrixTmp._23,
								matrixTmp._34 + matrixTmp._33,
								matrixTmp._44 + matrixTmp._43);
	m_Planes[5] = XMPlaneNormalize(m_Planes[5]);

	return;
}

// CheckPoint에서는 한 개의 point가 frustum 안에 있는 지 체크한다. 가장 일반적인 four checking 알고리즘이지만,
// 다른 체킹 방법에서도 잘 사용하면 아주 효과적이다.
// point를 받아, 6개의 plane 안에 있는 지 체크한다. 모든 plane의 안에 있으면 true, 바깥이면 false이다.

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;

	// 포인트가 6개의 plane 안에 있는 지 체크한다.
	for(i = 0; i < 6; i++)
	{ 
		if (XMVectorGetX( XMVector3Dot (m_Planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)
		{
		return false;
		}
	}

	return true;
}

// CheckCube는 cube의 각 8개 코너가 frustum 안에 있는지 체크한다. cube의 중앙점과 radius만 있으면 된다.
// 이 값으로 cube의 8개의 코너를 계산한다. 8개의 코너 중 하나라도 Plane 안에 있으면 true, 아니면 false를 리턴한다.
bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	// cube의 1 포안트라도 frustum에 있는지 체크한다.
	for (i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
		{
			continue;
		}
		return false;

	}

	return true;
}

// sphere의 radius가 frustum 안에 있는지 체크
bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	// sphere의 radius가 view frustum 안에 있는지 체크
	for (i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1.0f))) < -radius)
		{
			return false;
			
		}
	}

	return true;
}

// checkRectangle
bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))) >= 0.0f)
		{
			continue;
		}

		return false;

	}

	return true;
}


