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

// screen�� depth�� positionMatrix, viewmatrix�� ��� �� ������ ȣ��ȴ�. �ش� �������� view frustum�� �����.
// frustum�� plane 6���� �����.
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

// CheckPoint������ �� ���� point�� frustum �ȿ� �ִ� �� üũ�Ѵ�. ���� �Ϲ����� four checking �˰���������,
// �ٸ� üŷ ��������� �� ����ϸ� ���� ȿ�����̴�.
// point�� �޾�, 6���� plane �ȿ� �ִ� �� üũ�Ѵ�. ��� plane�� �ȿ� ������ true, �ٱ��̸� false�̴�.

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;

	// ����Ʈ�� 6���� plane �ȿ� �ִ� �� üũ�Ѵ�.
	for(i = 0; i < 6; i++)
	{ 
		if (XMVectorGetX( XMVector3Dot (m_Planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

// CheckCube�� cube�� �� 8�� �ڳʰ� frustum �ȿ� �ִ��� üũ�Ѵ�. cube�� �߾����� radius�� ������ �ȴ�.
// �� ������ cube�� 8���� �ڳʸ� ����Ѵ�. 8���� �ڳ� �� �ϳ��� Plane �ȿ� ������ true, �ƴϸ� false�� �����Ѵ�.
bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	// cube�� 1 ����Ʈ�� frustum�� �ִ��� üũ�Ѵ�.
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

// sphere�� radius�� frustum �ȿ� �ִ��� üũ
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


