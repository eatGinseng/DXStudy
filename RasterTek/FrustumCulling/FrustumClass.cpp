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
	float zMinimum, r;
	float x, y, z, w;
	XMMATRIX matrix;
	XMFLOAT4X4A matrixFF;

	XMFLOAT4X4A projectionMatrixFF;
	XMStoreFloat4x4A(&projectionMatrixFF, projectionMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projectionMatrixFF._43 / projectionMatrixFF._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrixFF._33 = r;
	projectionMatrixFF._43 = -r * zMinimum;

	projectionMatrix = XMLoadFloat4x4A(&projectionMatrixFF);

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMStoreFloat4x4A(&matrixFF, matrix);

	// Calculate near plane of frustum.
	x = matrixFF._14 + matrixFF._13;
	y = matrixFF._24 + matrixFF._23;
	z = matrixFF._34 + matrixFF._33;
	w = matrixFF._44 + matrixFF._43;
	m_Planes[0] = XMPlaneNormalize(XMVectorSet(x, y, z, w));

	// Calculate far plane of frustum.
	x = matrixFF._14 - matrixFF._13;
	y = matrixFF._24 - matrixFF._23;
	z = matrixFF._34 - matrixFF._33;
	w = matrixFF._44 - matrixFF._43;
 	m_Planes[1] = XMPlaneNormalize(XMVectorSet(x, y, z, w));

	// Calculate left plane of frustum.
	x = matrixFF._14 + matrixFF._11;
	y = matrixFF._24 + matrixFF._21;
	z = matrixFF._34 + matrixFF._31;
	w = matrixFF._44 + matrixFF._41;
	m_Planes[2] = XMPlaneNormalize(XMVectorSet(x, y, z, w));

	// Calculate right plane of frustum.
	x = matrixFF._14 - matrixFF._11;
	y = matrixFF._24 - matrixFF._21;
	z = matrixFF._34 - matrixFF._31;
	w = matrixFF._44 - matrixFF._41;
	m_Planes[3] = XMPlaneNormalize(XMVectorSet(x, y, z, w));

	// Calculate top plane of frustum.
	x = matrixFF._14 - matrixFF._12;
	y = matrixFF._24 - matrixFF._22;
	z = matrixFF._34 - matrixFF._32;
	w = matrixFF._44 - matrixFF._42;
	m_Planes[4] = XMPlaneNormalize(XMVectorSet(x, y, z, w));

	// Calculate bottom plane of frustum.
	x = matrixFF._14 + matrixFF._12;
	y = matrixFF._24 + matrixFF._22;
	z = matrixFF._34 + matrixFF._32;
	w = matrixFF._44 + matrixFF._42;
	m_Planes[5] = XMPlaneNormalize(XMVectorSet(x, y, z, w));

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
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter - radius), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter - radius), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter + radius), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter + radius), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter + radius), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter + radius), 0.0f))) >= 0.0f)
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

	// Check if the radius of the sphere is inside the view frustum.
	for (i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(m_Planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1))) < -radius)
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
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}
		if (XMVectorGetX(XMVector3Dot(m_Planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 0.0f))) >= 0.0f)
		{
			continue;
		}

		return false;

	}

	return true;
}


