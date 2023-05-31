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
	m_WallModel = 0;
	m_BathModel = 0;
	m_WaterModel = 0;

	m_RefractionTexture = 0;
	m_ReflectionTexture = 0;

	m_Light = 0;
	m_LightShader = 0;

	m_RefractionShader = 0;
	m_WaterShader = 0;

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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	
	char GroundFilename[128];
	strcpy_s(GroundFilename, "ground.txt");

	char GroundTgaName[128];
	strcpy_s(GroundTgaName, "ground01.tga");

	char WallFilename[128];
	strcpy_s(WallFilename, "wall.txt");

	char WallTgaName[128];
	strcpy_s(WallTgaName, "wall01.tga");

	char BathFileName[128];
	strcpy_s(BathFileName, "bath.txt");

	char BathTgaName[128];
	strcpy_s(BathTgaName, "marble01.tga");

	char WaterFileName[128];
	strcpy_s(WaterFileName, "water.txt");

	char WaterTgaName[128];
	strcpy_s(WaterTgaName, "water01.tga");

	// Create the ground model object.
	m_GroundModel = new ModelClass;
	if (!m_GroundModel)
	{
		return false;
	}

	// Initialize the ground model object.
	result = m_GroundModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), GroundTgaName, hwnd, GroundFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// Create the wall model object.
	m_WallModel = new ModelClass;
	if (!m_WallModel)
	{
		return false;
	}

	// Initialize the wall model object.
	result = m_WallModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), WallTgaName, hwnd, WallFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the wall model object.", L"Error", MB_OK);
		return false;
	}

	// Create the bath model object.
	m_BathModel = new ModelClass;
	if (!m_BathModel)
	{
		return false;
	}

	// Initialize the bath model object.
	result = m_BathModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), BathTgaName, hwnd, BathFileName);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bath model object.", L"Error", MB_OK);
		return false;
	}

	// Create the water model object.
	m_WaterModel = new ModelClass;
	if (!m_WaterModel)
	{
		return false;
	}

	// Initialize the water model object.
	result = m_WaterModel->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), WaterTgaName, hwnd, WaterFileName);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the water model object.", L"Error", MB_OK);
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

	// Create the refraction shader object.
	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader)
	{
		return false;
	}

	// Initialize the refraction shader object.
	result = m_RefractionShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the water shader object.
	m_WaterShader = new WaterShaderClass;
	if (!m_WaterShader)
	{
		return false;
	}

	// Initialize the water shader object.
	result = m_WaterShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the water shader object.", L"Error", MB_OK);
		return false;
	}

	// Create text object
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// text object 초기화
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		return false;
	}

	// cursor bitmap object 초기화
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

	// Set the height of the water.
	m_waterHeight = 2.75f;

	// Initialize the position of the water.
	m_waterTranslation = 0.0f;

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// TextureShader 초기화
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

	// Update the position of the water to simulate motion.
	m_waterTranslation += 0.001f;
	if (m_waterTranslation > 1.0f)
	{
		m_waterTranslation -= 1.0f;
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

	/// 먼저 Refraction을 텍스처에 렌더 한다. 그 다음 Reflection을 렌더 한다.
	/// 마지막으로 원래 장면을 렌더 한다. 이때 앞서 렌더한 두 텍스처를 사용한다.

	// Render the refraction of the scene to a texture.
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	result = RenderRefractionTexture();
	if (!result)
	{
		return false;
	}

	// Render the reflection of the scene to a texture.
	result = RenderReflectionToTexture();
	if (!result)
	{
		return false;
	}

	// Render the scene as normal to the back buffer.
	result = RenderScene();
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

	// Alpha blending 켜기
	m_D3D->TurnOnAlphaBlending();

	// text string 렌더하기
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

bool GraphicsClass::RenderReflectionToTexture()
{
	bool result;
	XMMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix, translateMatrix;
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->RenderReflection(m_waterHeight);
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	// Get the world and projection matrices from the d3d object.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the wall model will be rendered.
	translateMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	m_WallModel->Render(m_D3D->GetDeviceContext());
	XMVECTOR lightDirection = XMVectorSet(m_Light->GetDirection().x, m_Light->GetDirection().y, m_Light->GetDirection().z, 1.0f);

	// Render the wall model using the light shader and the reflection view matrix.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix,
		projectionMatrix, m_WallModel->GetTexture(), lightDirection,
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;

}

bool GraphicsClass::RenderRefractionTexture()
{
	bool result;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	static float textureTranslation = 0.0f;

	// Setup a clipping plane based on the height of the water to clip everything above it.
	// 0.1f 정도 offset 시킴 : 가장자리에 나타나는 artifact 때문..
	D3DXVECTOR4 dxClipPlane(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);
	XMVECTOR clipPlane = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&dxClipPlane));

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world and projection matrices.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the bath model will be rendered.
	XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);

	// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_BathModel->Render(m_D3D->GetDeviceContext());

	// Render the bath model using the light shader.
	result = m_RefractionShader->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_BathModel->GetTexture(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), clipPlane);

	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}


// 원래 하던 렌더를 여기에서 함
bool GraphicsClass::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix;
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

	XMVECTOR lightDirection = XMVectorSet(m_Light->GetDirection().x, m_Light->GetDirection().y, m_Light->GetDirection().z, 1.0f);

	// Render the ground model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_GroundModel->GetTexture(), lightDirection,
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Translate to where the wall model will be rendered.
	translationMatrix = XMMatrixTranslation(0.0f, 3.0f, 8.0f);
	worldMatrix = XMMatrixMultiply(translationMatrix, worldMatrix);

	// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_WallModel->Render(m_D3D->GetDeviceContext());

	// Render the wall model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_WallModel->GetTexture(), lightDirection,
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Translate to where the bath model will be rendered.'
	translationMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);

	// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_BathModel->Render(m_D3D->GetDeviceContext());

	// Render the bath model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_BathModel->GetTexture(), lightDirection,
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	translationMatrix = XMMatrixTranslation(0.0f, m_waterHeight, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);

	m_WaterModel->Render(m_D3D->GetDeviceContext());

	// Render the water model using the water shader.
	result = m_WaterShader->Render(m_D3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, reflectionMatrix, m_ReflectionTexture->GetShaderResourceView(),
		m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(),
		m_waterTranslation, 0.01f);
	if (!result)
	{
		return false;
	}

	return true;

}