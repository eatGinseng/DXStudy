#include "GraphicsClass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model - 0;
	m_TextureShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Direct3D object�� ����

	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Direct3D Object�� �ʱ�ȭ
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Camera ������Ʈ �����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// ī�޶��� �ʱ� ��ġ ����
	m_Camera->SetPosition(0.0f, 0.0f, 0.0f);

	// model ������Ʈ �����


	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	result = m_Model->Initialize(m_D3D->GetDevice(), (WCHAR*)L"../Engine/data/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// texture shader ������Ʈ ����
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// shader object �ʱ�ȭ
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize shader object.", L"Error", MB_OK);
		return false;
	}
	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	// graphics scene�� �����Ѵ�.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	// scene�� �����ϱ� ���� buffer�� clear �Ѵ�.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ġ�� ����� view matrix �� �����.
	m_Camera->Render();

	// camera�� d3d ������Ʈ���� world, view, projection ����� ��´�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// model�� vertex buffer�� index buffer�� �׷��Ƚ� ���������ο� ����־� �׸� �غ� �Ѵ�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// texture shader�� ����� ���� �׸���.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	// render �� scene�� present �Ѵ�.
	m_D3D->EndScene();

	return true;
}
