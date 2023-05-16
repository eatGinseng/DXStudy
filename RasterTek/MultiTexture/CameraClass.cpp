#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 10.0f;

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

// Build a reflection view matrix. 보통의 view matrix를 만드는 것과 비슷하지만, input으로 받은 height가 Y축 Plane의 높이로 동작한다.
// 이 height를 position.y 값을 반전시키는데 사용할 것이다. 이 매트릭스를 셰이더에서 사용한다.
// 이 function은 y 축 plane에만 적용되는 함수이다.

void CameraClass::RenderReflection(float height)
{
	XMFLOAT4 up, position, lookAt;
	XMVECTOR upV, positionV, lookAtV;
	float radians;

	// 위를 향하는 벡터 생성
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	up.w = 0.0f;

	upV = XMLoadFloat4(&up);

	// 월드공간 내의 Camera의 position을 셋업한다.
	// planar reflection을 위해, 카메라의 y position을 반전시킨다.
	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f);
	position.z = m_positionZ;
	position.w = 0.0f;

	positionV = XMLoadFloat4(&position);

	// rotation을 radian 으로 계산
	radians = m_rotationY * 0.0174532925f;

	// camera가 바라보는 곳
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_positionZ;
	lookAt.w = 0.0f;

	lookAtV = XMLoadFloat4(&lookAt);

	// 위 3개 벡터로 view matrix 생성
	m_reflectionViewMatrix = XMMatrixLookAtLH(positionV, lookAtV, upV);
}

XMMATRIX CameraClass::GetReflectionViewMatrix()
{
	return m_reflectionViewMatrix;
}

 
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