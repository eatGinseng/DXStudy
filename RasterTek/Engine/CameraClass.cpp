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

// Remder() 함수는 위치와 회전 값을 받아 view 행렬을 빌드하고 업데이트한다.
// 먼저 장면의 원점에서 카메라를 x, y, z 값에 따라 회전시킨다.
// 회전이 끝나면 position으로 이동시킨다. position, up, lookat 값이 제대로 셋팅되면
// 현재 카메라의 회전과 위치를 나타내는 view matrix를 만드는 함수를 호출한다.
void CameraClass::Render()
{

	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// 위를 향하는 벡터를 셋업한다.
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	// up.x = 0.0f;
	// up.y = 1.0f;
	// up.z = 0.0f;

	// 월드상의 카메라 위치를 셋업한다.
	position = XMVectorSet(m_positionX, m_positionY, m_positionZ, 1.0f);
//	position.x = m_positionX;
//	position.y = m_positionY;
//	position.z = m_positionZ;

	// 카메라가 어디를 바라보고 있을지..
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
//	lookAt.x = 0.0f;
//	lookAt.y = 0.0f;
//	lookAt.z = 1.0f;

	// roll, pitch, yaw 값을 radian 값으로 셋팅
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// pitch, yaw, roll 값으로 rotation matrix를 만든다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt 과 up을 rotation matrix로 회전시킨다.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// 카메라를 viewer 위치로 이동시킨다.
	lookAt = position + lookAt;

	// 마지막으로 업데이트된 3개의 벡터로 view matrix를 만든다.
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