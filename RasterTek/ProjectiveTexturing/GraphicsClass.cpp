////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_CubeModel = 0;
	m_GroundModel = 0;
	m_SphereModel = 0;

	m_Light = 0;

	m_ViewPoint = 0;
	m_ProjectionShader = 0;
	m_ProjectionTexture = 0;

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
	int downSampleWidth, downSampleHeight;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 4.0f, -10.0f);
	m_Camera->SetRotation(8.0f, 0.0f, 0.0f);
	m_Camera->Render();

	char textureFilename1[128];
	strcpy_s(textureFilename1, "wall01.tga");

	char modelFilename1[128];
	ZeroMemory(modelFilename1, sizeof(char) * 128);
	strcpy_s(modelFilename1, "cube.txt");

	char modelFilename2[128];
	ZeroMemory(modelFilename2, sizeof(char) * 128);
	strcpy_s(modelFilename2, "sphere.txt");

	char modelFilename3[128];
	ZeroMemory(modelFilename3, sizeof(char) * 128);
	strcpy_s(modelFilename3, "plane01.txt");

	m_CubeModel = new ModelClass;
	if (!m_CubeModel)
	{
		return false;
	}

	result = m_CubeModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(),textureFilename1, hwnd, modelFilename1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// Set the position for the cube model.
	m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);

	m_SphereModel = new ModelClass;
	if (!m_SphereModel)
	{
		return false;
	}

	result = m_SphereModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), textureFilename1, hwnd, modelFilename2);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}

	// Set the position for the sphere model.
	m_SphereModel->SetPosition(2.0f, 2.0f, 0.0f);

	// Create the ground model object.
	m_GroundModel = new ModelClass;
	if (!m_GroundModel)
	{
		return false;
	}

	// Initialize the ground model object.
	result = m_GroundModel->Initialize(m_D3D->GetDevice(),m_D3D->GetDeviceContext(), textureFilename1, hwnd, modelFilename3);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// Set the position for the ground model.
	m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetPosition(5.0f, 8.0f, -5.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);

	// Create the projection shader object.
	m_ProjectionShader = new ProjectionShaderClass;
	if (!m_ProjectionShader)
	{
		return false;
	}

	// Initialize the projection shader object.
	result = m_ProjectionShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the projection shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the projection texture object.
	m_ProjectionTexture = new TextureClass;
	if (!m_ProjectionTexture)
	{
		return false;
	}

	char textureFilename2[128];
	strcpy_s(textureFilename2, "grid01.tga");

	// Initialize the projection texture object.
	result = m_ProjectionTexture->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), textureFilename2, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the projection texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the view point object.
	m_ViewPoint = new ViewPointClass;
	if (!m_ViewPoint)
	{
		return false;
	}

	// Initialize the view point object.
	m_ViewPoint->SetPosition(2.0f, 5.0f, -2.0f);
	m_ViewPoint->SetLookAt(0.0f, 0.0f, 0.0f);
	m_ViewPoint->SetProjectionParameters((float)(D3DX_PI / 2.0f), 1.0f, 0.1f, 100.0f);
	m_ViewPoint->GenerateViewMatrix();
	m_ViewPoint->GenerateProjectionMatrix();

	return true;
}


	void GraphicsClass::Shutdown()
	{
	
		// Release the view point object.
		if (m_ViewPoint)
		{
			delete m_ViewPoint;
			m_ViewPoint = 0;
		}

		// Release the projection texture object.
		if (m_ProjectionTexture)
		{
			m_ProjectionTexture->Shutdown();
			delete m_ProjectionTexture;
			m_ProjectionTexture = 0;
		}

		// Release the projection shader object.
		if (m_ProjectionShader)
		{
			m_ProjectionShader->Shutdown();
			delete m_ProjectionShader;
			m_ProjectionShader = 0;
		}

		// Release the light object.
		if (m_Light)
		{
			delete m_Light;
			m_Light = 0;
		}

		// Release the ground model object.
		if (m_GroundModel)
		{
			m_GroundModel->Shutdown();
			delete m_GroundModel;
			m_GroundModel = 0;
		}

		// Release the sphere model object.
		if (m_SphereModel)
		{
			m_SphereModel->Shutdown();
			delete m_SphereModel;
			m_SphereModel = 0;
		}

		// Release the cube model object.
		if (m_CubeModel)
		{
			m_CubeModel->Shutdown();
			delete m_CubeModel;
			m_CubeModel = 0;
		}

		// Release the camera object.
		if (m_Camera)
		{
			delete m_Camera;
			m_Camera = 0;
		}

		// Release the D3D object.
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


	// Update the position of the light.
	m_Light->SetPosition(0.0f, 8.0f, -5.0f);

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix, projectionMatrix, translationMat, viewMatrix2, projectionMatrix2;
	bool result;
	float posX, posY, posZ;


	// Clear the buffers to begin the scene. clear with fog color.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the view and projection matrices from the view point object.
	m_ViewPoint->GetViewMatrix(viewMatrix2);
	m_ViewPoint->GetProjectionMatrix(projectionMatrix2);

	// 이제 각 모델을 Shadow map shader와 light 매트릭스들, 그리고 shadow map 텍스처로 렌더한다.
	// Setup the translation matrix for the cube model.
	m_CubeModel->GetPosition(posX, posY, posZ);
	translationMat = XMMatrixTranslation(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMat);

	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_CubeModel->Render(m_D3D->GetDeviceContext());

	// Render the model using the shadow shader.
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetDirection(),
		 m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the sphere model.
	m_SphereModel->GetPosition(posX, posY, posZ);
	translationMat = XMMatrixTranslation(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMat);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SphereModel->Render(m_D3D->GetDeviceContext());
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetDirection(),
		m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the ground model.
	m_GroundModel->GetPosition(posX, posY, posZ);
	translationMat = XMMatrixTranslation(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMat);

	// Render the ground model using the shadow shader.
	m_GroundModel->Render(m_D3D->GetDeviceContext());
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetDirection(),
		m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

