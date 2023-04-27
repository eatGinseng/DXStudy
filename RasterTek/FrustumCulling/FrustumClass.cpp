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
	float zMininum, r;
	XMMATRIX matrix;

	XMFLOAT4X4 projectionTmp;
	XMStoreFloat4x4(&projectionTmp, projectionMatrix);

	// frustum�� �ּ� z �Ÿ��� ����Ѵ�.
	zMininum = -projectionTmp._43 / projectionTmp._33;
	r = screenDepth / (screenDepth - zMininum);
	projectionTmp._33 = r;
	projectionTmp._32 = -r * zMininum;

	projectionMatrix = XMLoadFloat4x4(&projectionTmp);

	// view matrix�� ������Ʈ�� projection matrix�� frustum matrix�� �����.
	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// frustum�� near plane ����
	XMFLOAT4X4 matrixTmp;
	XMStoreFloat4x4(&matrixTmp, matrix);

	m_Planes[0] = XMVectorSet(	matrixTmp._14 + matrixTmp._13,
								matrixTmp._24 + matrixTmp._23,
								matrixTmp._34 + matrixTmp._33,
								matrixTmp._44 + matrixTmp._43 );

	m_Planes[0] = XMPlaneNormalize(m_Planes[0]);

	// Far Plane ����
	m_Planes[1] = XMVectorSet(	matrixTmp._14 - matrixTmp._13,
								matrixTmp._24 - matrixTmp._23,
								matrixTmp._34 - matrixTmp._33,
								matrixTmp._44 - matrixTmp._43);
	m_Planes[1] = XMPlaneNormalize(m_Planes[1]);

	// Left plane ����
	m_Planes[2] = XMVectorSet(	matrixTmp._14 + matrixTmp._13,
								matrixTmp._24 + matrixTmp._23,
								matrixTmp._34 + matrixTmp._33,
								matrixTmp._44 + matrixTmp._43);
	m_Planes[2] = XMPlaneNormalize(m_Planes[2]);

	// Right Plane ����
	m_Planes[3] = XMVectorSet(	matrixTmp._14 - matrixTmp._13,
								matrixTmp._24 - matrixTmp._23,
								matrixTmp._34 - matrixTmp._33,
								matrixTmp._44 - matrixTmp._43);
	m_Planes[3] = XMPlaneNormalize(m_Planes[3]);

	// top plane ����
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

// sphere�� radius�� frustum �ȿ� �ִ��� üũ
bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	// sphere�� radius�� view frustum �ȿ� �ִ��� üũ
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


