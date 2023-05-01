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
	m_Model = 0;

	m_MultiTextureShader = 0;
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
	XMMATRIX baseViewMatrix;

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

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// TextureShader 초기화
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		return false;
	}

	// cursor bitmap object 초기화
	m_Cursor = new BitmapClass;
	if (!m_Cursor)
	{
		return false;
	}

	// char cursorTextureFilename[128];
	// char* cursorTextureFilename = new char[128];
	// strcpy_s(cursorTextureFilename, "Cursor.tga");

	result = m_Cursor->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), screenWidth, screenHeight, 50, 50, "Cursor.tga", hwnd);
	if (!result)
	{
		return false;
	}

	char textureFilename1[128];
	strcpy_s(textureFilename1, "seafloor.tga");

	char textureFilename2[128];
	strcpy_s(textureFilename2, "pop.tga");

	char modelFilename[128];
	ZeroMemory(modelFilename, sizeof(char) * 128);
	strcpy_s(modelFilename, "ChamferredBox.txt");

	// ModelClass 초기화
	m_Model = new ModelClass;
	result = m_Model->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), textureFilename1, textureFilename2, hwnd, modelFilename);

	m_MultiTextureShader = new MultiTextureShaderClass;
	result = m_MultiTextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the multitexture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the multiTextureShaderClass
	if (m_MultiTextureShader)
	{
		m_MultiTextureShader->Shutdown();
		delete m_MultiTextureShader;
		m_MultiTextureShader = 0;
	}

	// Release the cursor bitmap
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

	// Set the position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);


	m_Model->Render(m_D3D->GetDeviceContext());

	// multiTextureShader로 model object를 그린다.
	m_MultiTextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());


	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

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
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Cursor->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Cursor->GetTexture());
	if (!result)
	{
		return false;
	}

	// alpha blending off
	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}