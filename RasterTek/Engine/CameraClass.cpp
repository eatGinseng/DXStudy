#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}

// Remder() �Լ��� ��ġ�� ȸ�� ���� �޾� view ����� �����ϰ� ������Ʈ�Ѵ�.
// ���� ����� �������� ī�޶� x, y, z ���� ���� ȸ����Ų��.
// ȸ���� ������ position���� �̵���Ų��. position, up, lookat ���� ����� ���õǸ�
// ���� ī�޶��� ȸ���� ��ġ�� ��Ÿ���� view matrix�� ����� �Լ��� ȣ���Ѵ�.
void CameraClass::Render()
{

	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// ���� ���ϴ� ���͸� �¾��Ѵ�.
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	// up.x = 0.0f;
	// up.y = 1.0f;
	// up.z = 0.0f;

	// ������� ī�޶� ��ġ�� �¾��Ѵ�.
	position = XMVectorSet(m_positionX, m_positionY, m_positionZ, 1.0f);
//	position.x = m_positionX;
//	position.y = m_positionY;
//	position.z = m_positionZ;

	// ī�޶� ��� �ٶ󺸰� ������..
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
//	lookAt.x = 0.0f;
//	lookAt.y = 0.0f;
//	lookAt.z = 1.0f;

	// roll, pitch, yaw ���� radian ������ ����
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// pitch, yaw, roll ������ rotation matrix�� �����.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt �� up�� rotation matrix�� ȸ����Ų��.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// ī�޶� viewer ��ġ�� �̵���Ų��.
	lookAt = position + lookAt;

	// ���������� ������Ʈ�� 3���� ���ͷ� view matrix�� �����.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);


}

// 
void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}