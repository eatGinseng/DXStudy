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


	m_TextureShader = 0;
	m_SmallWindow = 0;
	m_FullScreenWindow = 0;

	m_HorizontalBlurShader = 0;
	m_VerticalBlurShader = 0;

	m_GlowTexture = 0;
	m_DownSampleTexture = 0;
	m_UpsampleTexture = 0;
	m_SceneTexture = 0;

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

	downSampleWidth = screenWidth / 2;
	downSampleHeight = screenHeight / 2;

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

	m_Camera->GetViewMatrix(baseViewMatrix);

	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow)
	{
		return false;
	}

	result = m_SmallWindow->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Small Window object.", L"Error", MB_OK);
		return false;
	}

	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	result = m_FullScreenWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Ortho Window object.", L"Error", MB_OK);
		return false;
	}

	m_GlowTexture = new RenderToTextureClass;
	if (!m_GlowTexture)
	{
		return false;
	}

	result = m_GlowTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Glow Texture object.", L"Error", MB_OK);
		return false;
	}

	m_DownSampleTexture = new RenderToTextureClass;
	if (!m_DownSampleTexture)
	{
		return false;
	}

	result = m_DownSampleTexture->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the DownSample Texture object.", L"Error", MB_OK);
		return false;
	}

	m_HorizontalBlurTexture = new RenderToTextureClass;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	result = m_HorizontalBlurTexture->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Horizontal Blur Texture object.", L"Error", MB_OK);
		return false;
	}

	m_VerticalBlurTexture = new RenderToTextureClass;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	result = m_VerticalBlurTexture->Initialize(m_D3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical Blur Texture object.", L"Error", MB_OK);
		return false;
	}

	m_UpsampleTexture = new RenderToTextureClass;
	if (!m_UpsampleTexture)
	{
		return false;
	}

	result = m_UpsampleTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		return false;
	}

	m_SceneTexture = new RenderToTextureClass;
	if (!m_SceneTexture)
	{
		return false;
	}

	result = m_SceneTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		return false;
	}

	m_FinalTexture = new RenderToTextureClass;
	if (!m_FinalTexture)
	{
		return false;
	}

	result = m_FinalTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		return false;
	}

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

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}

	result = m_HorizontalBlurShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader)
	{
		return false;
	}

	result = m_VerticalBlurShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	m_GlowShader = new GlowShaderClass;
	if (!m_GlowShader)
	{
		return false;
	}

	result = m_GlowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
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

		if (m_SceneTexture)
		{
			m_SceneTexture->Shutdown();
			delete m_SceneTexture;
			m_SceneTexture = 0;
		}

		if (m_GlowTexture)
		{
			m_GlowTexture->Shutdown();
			delete m_GlowTexture;
			m_GlowTexture = 0;
		}

		if (m_UpsampleTexture)
		{
			m_UpsampleTexture->Shutdown();
			delete m_UpsampleTexture;
			m_UpsampleTexture = 0;
		}

		if (m_HorizontalBlurTexture)
		{
			m_HorizontalBlurTexture->Shutdown();
			delete m_HorizontalBlurTexture;
			m_HorizontalBlurTexture = 0;
		}

		if (m_VerticalBlurTexture)
		{
			m_VerticalBlurTexture->Shutdown();
			delete m_VerticalBlurTexture;
			m_VerticalBlurTexture = 0;
		}

		if (m_DownSampleTexture)
		{
			m_DownSampleTexture->Shutdown();
			delete m_DownSampleTexture;
			m_DownSampleTexture = 0;
		}

		if (m_FinalTexture)
		{
			m_FinalTexture->Shutdown();
			delete m_FinalTexture;
			m_FinalTexture = 0;
		}

		if (m_LightShader)
		{
			m_LightShader->Shutdown();
			delete m_LightShader;
			m_LightShader = 0;
		}

		if (m_GlowShader)
		{
			m_GlowShader->Shutdown();
			delete m_GlowShader;
			m_GlowShader = 0;
		}

		if (m_TextureShader)
		{
			m_TextureShader->Shutdown();
			delete m_TextureShader;
			m_TextureShader = 0;
		}

		// Release the projection shader object.
		if (m_ProjectionShader)
		{
			m_ProjectionShader->Shutdown();
			delete m_ProjectionShader;
			m_ProjectionShader = 0;
		}

		if (m_VerticalBlurShader)
		{
			m_VerticalBlurShader->Shutdown();
			delete m_VerticalBlurShader;
			m_VerticalBlurShader = 0;
		}

		if (m_HorizontalBlurShader)
		{
			m_HorizontalBlurShader->Shutdown();
			delete m_HorizontalBlurShader;
			m_HorizontalBlurShader = 0;
		}

		// Release the light object.
		if (m_Light)
		{
			delete m_Light;
			m_Light = 0;
		}

		if (m_SmallWindow)
		{
			m_SmallWindow->Shutdown();
			delete m_SmallWindow;
			m_SmallWindow = 0;
		}

		if (m_FullScreenWindow)
		{
			m_FullScreenWindow->Shutdown();
			delete m_FullScreenWindow;
			m_FullScreenWindow = 0;
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

bool GraphicsClass::RenderGlowTexture()
{

	XMMATRIX worldMatrix, viewMatrix, orthoMatrix, projectionMatrix, translationMat, viewMatrix2, projectionMatrix2;
	bool result;
	float posX, posY, posZ;

	m_GlowTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	m_GlowTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

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
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetPosition(),
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
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetPosition(),
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
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetPosition(),
		m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();


}

bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	m_DownSampleTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_DownSampleTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_DownSampleTexture->GetOrthoMatrix(orthoMatrix);

	m_DownSampleTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_DownSampleTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_SmallWindow->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_GlowTexture->GetShaderResourceView() );
	if (!result)
	{
		return false;
	}

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderHorizontalBlurTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;

	m_HorizontalBlurTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_HorizontalBlurTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	screenSizeX = m_HorizontalBlurTexture->GetTextureWidth();

	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	m_SmallWindow->Render(m_D3D->GetDeviceContext());
	result = m_HorizontalBlurShader->Render(m_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_DownSampleTexture->GetShaderResourceView(), screenSizeX);

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderVerticalBlurTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;

	m_VerticalBlurTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_VerticalBlurTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	screenSizeY = m_VerticalBlurTexture->GetTextureHeight();

	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	m_SmallWindow->Render(m_D3D->GetDeviceContext());
	result = m_VerticalBlurShader->Render(m_D3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::UpsampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	m_UpsampleTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_UpsampleTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	m_UpsampleTexture->GetOrthoMatrix(orthoMatrix);

	m_FullScreenWindow->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_VerticalBlurTexture->GetShaderResourceView());

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderSceneToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translationMat;
	float posX, posY, posZ;
	bool result;

	m_SceneTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_SceneTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Setup the translation matrix for the cube model.
	m_CubeModel->GetPosition(posX, posY, posZ);
	translationMat = XMMatrixTranslation(posX, posY, posZ);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMat);

	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_CubeModel->Render(m_D3D->GetDeviceContext());

	// Render the model using the shadow shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetDirection(),
		m_Light->GetDiffuseColor(), m_Light->GetAmbientColor());
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
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetDirection(),
		m_Light->GetDiffuseColor(), m_Light->GetAmbientColor());
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
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_CubeModel->GetTexture(), m_Light->GetDirection(),
		m_Light->GetDiffuseColor(), m_Light->GetAmbientColor());
	if (!result)
	{
		return false;
	}

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

}

bool GraphicsClass::RenderGlowMapToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	m_FinalTexture->SetRenderTarget(m_D3D->GetDeviceContext());
	m_FinalTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	m_FinalTexture->GetOrthoMatrix(orthoMatrix);

	m_FullScreenWindow->Render(m_D3D->GetDeviceContext());
	result = m_GlowShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_SceneTexture->GetShaderResourceView(), m_GlowTexture->GetShaderResourceView(), m_UpsampleTexture->GetShaderResourceView(), 5.0f);

	m_D3D->SetBackBufferRenderTarget();

	m_D3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;
	float posX, posY, posZ;

	RenderGlowTexture();

	RenderSceneToTexture();

	DownSampleTexture();

	RenderHorizontalBlurTexture();

	RenderVerticalBlurTexture();

	UpsampleTexture();

	RenderGlowMapToTexture();

	// Clear the buffers to begin the scene. clear with fog color.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_FinalTexture->GetOrthoMatrix(orthoMatrix);

	m_FullScreenWindow->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_FinalTexture->GetShaderResourceView());

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}


