////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;

	m_Text = 0;

	m_Cursor = 0;
	m_TextureShader = 0;
	m_GroundModel = 0;
	m_CubeModel = 0;

	m_RefractionTexture = 0;
	m_ReflectionTexture = 0;

	m_Light = 0;
	m_LightShader = 0;

	m_PointLightShader = 0;
	m_PointLight1 = 0;
	m_PointLight2 = 0;
	m_PointLight3 = 0;
	m_PointLight4 = 0;

	m_GlassShader = 0;

	m_Bitmap = 0;

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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	
	char GroundFilename[128];
	strcpy_s(GroundFilename, "ground.txt");

	char GroundTgaName[128];
	strcpy_s(GroundTgaName, "ground01.tga");

	char CubeFilename[128];
	strcpy_s(CubeFilename, "ChamferredBox.txt");

	char CubeTgaName[128];
	strcpy_s(CubeTgaName, "seafloor.tga");

	char CubeNormalTgaName[128];
	strcpy_s(CubeNormalTgaName, "noiseNormal.tga");

	// Create the ground model object.
	m_GroundModel = new ModelClass;
	if (!m_GroundModel)
	{
		return false;
	}

	// Initialize the ground model object.
	result = m_GroundModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), GroundTgaName, CubeNormalTgaName, hwnd, GroundFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// Create the Cube object
	m_CubeModel = new ModelClass;
	if (!m_CubeModel)
	{
		return false;
	}
	
	// Initialize the Cube object
	result = m_CubeModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), CubeTgaName, CubeNormalTgaName, hwnd, CubeFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Cube model object.", L"Error", MB_OK);
		return false;
	}
	/// Create the light object.
	
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 1.0f, 0.5f);

	/// Create the render to texture objects

	// Create the refraction render to texture object.
	m_RefractionTexture = new RenderToTextureClass;
	if (!m_RefractionTexture)
	{
		return false;
	}

	// Initialize the refraction render to texture object.
	result = m_RefractionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the reflection render to texture object.
	m_ReflectionTexture = new RenderToTextureClass;
	if (!m_ReflectionTexture)
	{
		return false;
	}

	// Initialize the reflection render to texture object.
	result = m_ReflectionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the reflection render to texture object.", L"Error", MB_OK);
		return false;
	}

	/// Initialize 3 Shaders
	
	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_PointLightShader = new PointLightShaderClass;
	if (!m_PointLightShader)
	{
		return false;
	}

	result = m_PointLightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Point Light shader object.", L"Error", MB_OK);
		return false;
	}

	/// Create Point Lights
	// Create the first light object.
	m_PointLight1 = new PointLightClass;
	if (!m_PointLight1)
	{
		return false;
	}

	// Initialize the first light object.
	m_PointLight1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_PointLight1->SetPosition(-3.0f, 7.0f, 3.0f);

	// Create the second light object.
	m_PointLight2 = new PointLightClass;
	if (!m_PointLight2)
	{
		return false;
	}

	// Initialize the second light object.
	m_PointLight2->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_PointLight2->SetPosition(3.0f, 7.0f, 3.0f);

	// Create the third light object.
	m_PointLight3 = new PointLightClass;
	if (!m_PointLight3)
	{
		return false;
	}

	// Initialize the third light object.
	m_PointLight3->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_PointLight3->SetPosition(-3.0f, 7.0f, -3.0f);

	// Create the fourth light object.
	m_PointLight4 = new PointLightClass;
	if (!m_PointLight4)
	{
		return false;
	}

	// Initialize the fourth light object.
	m_PointLight4->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_PointLight4->SetPosition(3.0f, 7.0f, -3.0f);

	// Create the glass shader object.
	m_GlassShader = new GlassShaderClass;
	if (!m_GlassShader)
	{
		return false;
	}

	// Initialize the glass shader object.
	result = m_GlassShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the glass shader object.", L"Error", MB_OK);
		return false;
	}

	// Create text object
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// text object �ʱ�ȭ
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		return false;
	}

	// cursor bitmap object �ʱ�ȭ
	m_Cursor = new BitmapClass;
	if (!m_Cursor)
	{
		return false;
	}

	result = m_Cursor->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), screenWidth, screenHeight, 50, 50, "Cursor.tga", hwnd);
	if (!result)
	{
		return false;
	}


	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// TextureShader �ʱ�ȭ
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}

	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{

	// Release the glass shader object.
	if (m_GlassShader)
	{
		m_GlassShader->Shutdown();
		delete m_GlassShader;
		m_GlassShader = 0;
	}

	// Release the bitmap object.
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
	// Release the cursor bitmap
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the light objects.
	if (m_PointLight1)
	{
		delete m_PointLight1;
		m_PointLight1 = 0;
	}

	if (m_PointLight2)
	{
		delete m_PointLight2;
		m_PointLight2 = 0;
	}

	if (m_PointLight3)
	{
		delete m_PointLight3;
		m_PointLight3 = 0;
	}

	if (m_PointLight4)
	{
		delete m_PointLight4;
		m_PointLight4 = 0;
	}

	// Release the light shader object.
	if (m_PointLightShader)
	{
		m_PointLightShader->Shutdown();
		delete m_PointLightShader;
		m_PointLightShader = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the light
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}


	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the cursor bitmap
	if (m_Cursor)
	{
		m_Cursor->Shutdown();
		delete m_Cursor;
		m_Cursor = 0;
	}

	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the ground and cube Model.
	if (m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	}

	if (m_CubeModel)
	{
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame(int fps, int cpu, float frameTime, int mouseX, int mouseY)
{
	bool result;


	// Set the location of the mouse.
	result = m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	result = m_Cursor->SetMousePosition(mouseX, mouseY);
	if (!result)
	{
		return false;
	}

	result = m_Text->SetFps(fps, m_D3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	result = m_Text->SetCpu(cpu, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Set the position and rotation of the camera.
	m_Camera->SetPosition(-10.0f, 6.0f, -10.0f);
	m_Camera->SetRotation(0.0f, 45.0f, 0.0f);

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	bool result;

	/// ���� Refraction�� �ؽ�ó�� ���� �Ѵ�. �� ���� Reflection�� ���� �Ѵ�.
	/// ���������� ���� ����� ���� �Ѵ�. �̶� �ռ� ������ �� �ؽ�ó�� ����Ѵ�.


	result = RenderRefractionTexture();
	if (!result)
	{
		return false;
	}

	// Render the refraction of the scene to a texture.
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the scene as normal to the back buffer.
	result = RenderScene(rotation);
	if (!result)
	{
		return false;
	}

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Alpha blending �ѱ�
	m_D3D->TurnOnAlphaBlending();

	// text string �����ϱ�
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = m_Cursor->Render(m_D3D->GetDeviceContext(), m_Cursor->mouseX, m_Cursor->mouseY);
	if (!result)
	{
		return false;
	}

	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Cursor->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, m_Cursor->GetTexture());
	if (!result)
	{
		return false;
	}

	// alpha blending off
	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	m_D3D->EndScene();
	return true;
}

bool GraphicsClass::RenderRefractionTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Light Color and Position Arrays
	XMVECTOR diffuseColor[4];
	XMVECTOR lightPosition[4];

	// Create the diffuse color array from the four light colors.
	diffuseColor[0] = m_PointLight1->GetDiffuseColor();
	diffuseColor[1] = m_PointLight2->GetDiffuseColor();
	diffuseColor[2] = m_PointLight3->GetDiffuseColor();
	diffuseColor[3] = m_PointLight4->GetDiffuseColor();

	// Create the light position array from the four light positions.
	lightPosition[0] = m_PointLight1->GetPosition();
	lightPosition[1] = m_PointLight2->GetPosition();
	lightPosition[2] = m_PointLight3->GetPosition();
	lightPosition[3] = m_PointLight4->GetPosition();

	bool result;

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	// Get the world and projection matrices.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the bath model will be rendered.
	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);

	// Put the ground model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_GroundModel->Render(m_D3D->GetDeviceContext());

	// Render the ground model using the light shader.
	result = m_PointLightShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_GroundModel->GetTexture(), diffuseColor, lightPosition);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}


// ���� �ϴ� ������ ���⿡�� ��
bool GraphicsClass::RenderScene(float rotation)
{

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

	// Light Color and Position Arrays
	XMVECTOR diffuseColor[4];
	XMVECTOR lightPosition[4];

	// Create the diffuse color array from the four light colors.
	diffuseColor[0] = m_PointLight1->GetDiffuseColor();
	diffuseColor[1] = m_PointLight2->GetDiffuseColor();
	diffuseColor[2] = m_PointLight3->GetDiffuseColor();
	diffuseColor[3] = m_PointLight4->GetDiffuseColor();

	// Create the light position array from the four light positions.
	lightPosition[0] = m_PointLight1->GetPosition();
	lightPosition[1] = m_PointLight2->GetPosition();
	lightPosition[2] = m_PointLight3->GetPosition();
	lightPosition[3] = m_PointLight4->GetPosition();

	// Set the refraction scale for the glass shader.
	float refractionScale = 0.01f;

	bool result;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the camera reflection view matrix.
	// reflectionMatrix = m_Camera->GetReflectionViewMatrix();
	m_D3D->GetOrthoMatrix(orthoMatrix);

	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);
	m_GroundModel->Render(m_D3D->GetDeviceContext());

	// Render the ground model using the light shader.
	result = m_PointLightShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_GroundModel->GetTexture(), diffuseColor, lightPosition);
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Translate to where the bath model will be rendered.'
	translationMatrix = XMMatrixTranslation(0.0f, 4.0f, 0.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(rotation);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);

	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_CubeModel->Render(m_D3D->GetDeviceContext());

	// Render the Cube model using the light shader.
	result = m_GlassShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_CubeModel->GetTexture(), m_CubeModel->GetNormalMap(), m_RefractionTexture->GetShaderResourceView(), refractionScale);

	if (!result)
	{
		return false;
	}

	return true;

}