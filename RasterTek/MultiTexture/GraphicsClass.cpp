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

	m_Plane = 0;

	m_RenderTexture = 0;

	m_Light = 0;

	m_MultiTextureShader = 0;

//	m_ReflectionShader = 0;

	m_Bitmap = 0;
	m_FadeShader = 0;
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
	

	// Create the light object
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	m_Light->SetDiffuseColor(1.0f, 1.0f, 0.7f, 1.0f);
	m_Light->SetDirection(0.6f, 0.0f, 0.5f);

	// Create the render to texture object


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

	char textureFilename3[128];
	strcpy_s(textureFilename3, "alpha.tga");

	char textureFilename4[128];
	strcpy_s(textureFilename4, "normal.tga");

	char modelFilename[128];
	ZeroMemory(modelFilename, sizeof(char) * 128);
	strcpy_s(modelFilename, "ChamferredBox.txt");

	char planeModelFilename[128];
	ZeroMemory(planeModelFilename, sizeof(char) * 128);
	strcpy_s(planeModelFilename, "plane.txt");

	// ModelClass 초기화
	m_Model = new ModelClass;
	result = m_Model->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), textureFilename1, textureFilename2, textureFilename3, textureFilename4, hwnd, modelFilename);
	if (!result)
	{
		return false;
	}

	// Plane model 초기화
/*	m_Plane = new ModelClass;
	result = m_Plane->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), textureFilename2, textureFilename2, textureFilename3, textureFilename4, hwnd, planeModelFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}
*/
	m_RenderTexture = new RenderToTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// initialize render to texture object
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	m_MultiTextureShader = new MultiTextureShaderClass;
	result = m_MultiTextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the multitexture shader object.", L"Error", MB_OK);
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

	// Set the fade in time to 3000 milliseconds.
	m_fadeInTime = 3000.0f;

	// Initialize the accumulated time to zero milliseconds.
	m_accumulatedTime = 0;

	// Initialize the fade percentage to zero at first so the scene is black.
	m_fadePercentage = 0;

	// Set the fading in effect to not done.
	m_fadeDone = false;

	// Create the fade shader object.
	m_FadeShader = new FadeShaderClass;
	if (!m_FadeShader)
	{
		return false;
	}

	// Initialize the fade shader object.
	result = m_FadeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the fade shader object.
	if (m_FadeShader)
	{
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
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

	if (m_RenderTexture)
	{
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the light
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the multiTextureShaderClass
	if (m_MultiTextureShader)
	{
		m_MultiTextureShader->Shutdown();
		delete m_MultiTextureShader;
		m_MultiTextureShader = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release Plane Model
	if (m_Plane)
	{
		m_Plane->Shutdown();
		delete m_Plane;
		m_Plane = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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
	
	if (!m_fadeDone)
	{
		// Update the accumulated time with the extra frame time addition.
		m_accumulatedTime += frameTime;

		// While the time goes on increase the fade in amount by the time that is passing each frame.
		if (m_accumulatedTime < m_fadeInTime)
		{
			// Calculate the percentage that the screen should be faded in based on the accumulated time.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}
		else
		{
			// If the fade in time is complete then turn off the fade effect and render the scene normally.
			m_fadeDone = true;

			// Set the percentage to 100%.
			m_fadePercentage = 1.0f;
		}
	}

	// Set the position of the camera.
	m_Camera->SetPosition(0.0f, 1.0f, -6.0f);
	m_Camera->SetRotation(12.0f, 0.0f, 0.0f);

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float fogColor;

	bool result;


	static float rotation = 0.0f;


	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Set the color of the fog to grey.
	fogColor = 0.5f;

	// Clear the buffers to begin the scene. clear with fog color.
	m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

	if (m_fadeDone)
	{
		// If fading in is complete then render the scene normally using the back buffer.
		result = RenderScene();
		if (!result)
		{
			return false;
		}
	}
	else
	{
		// If fading in is not complete then render the scene to a texture and fade that texture in.
		result = RenderToTexture(rotation);
		if (!result)
		{
			return false;
		}

		result = RenderFadingScene();
		if (!result)
		{
			return false;
		}

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


	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::RenderToTexture(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float fogStart, fogEnd, blendAmount;
	XMVECTOR clipPlane;
	static float textureTranslation = 0.0f;
	
	// Setup a clipping plane.
	clipPlane = XMVectorSet(0.0f, -2.0f, 0.0f, 0.5f);

	// Set the start and end of the fog.
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// Set the blending amount to 50%.
	blendAmount = 0.5f;

	// Increment the texture translation position.
	textureTranslation += 0.0001f;
	if (textureTranslation > 1.0f)
	{
		textureTranslation -= 1.0f;
	}

	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world and projection matrices.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	XMMATRIX rotationMatrix = XMMatrixRotationY(rotation * 0.05f);

	m_Model->Render(m_D3D->GetDeviceContext());

	// Render the model using the texture shader and the reflection view matrix.
	m_MultiTextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), XMMatrixMultiply(worldMatrix, rotationMatrix), viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), fogStart, fogEnd, clipPlane, textureTranslation);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenderFadingScene()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0);
	if (!result)
	{
		return false;
	}

	// Render the bitmap using the fade shader.
	result = m_FadeShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_RenderTexture->GetShaderResourceView(), m_fadePercentage);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

// 원래 하던 렌더를 여기에서 함
bool GraphicsClass::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix;
	float fogStart, fogEnd, blendAmount;
	XMVECTOR clipPlane;
	static float textureTranslation = 0.0f;

	bool result;

	// Setup a clipping plane.
	clipPlane = XMVectorSet(0.0f, -6.0f, 0.0f, 0.5f);

	// Set the start and end of the fog.
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// Set the blending amount to 50%.
	blendAmount = 0.5f;

	// Increment the texture translation position.
	textureTranslation += 0.0001f;
	if (textureTranslation > 1.0f)
	{
		textureTranslation -= 1.0f;
	}

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the camera reflection view matrix.
//	reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	m_D3D->GetOrthoMatrix(orthoMatrix);

	XMMATRIX rotationMatrix = XMMatrixRotationY(45.0f);
	m_Model->Render(m_D3D->GetDeviceContext());

	// multiTextureShader로 model object를 그린다.
	m_MultiTextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), XMMatrixMultiply(worldMatrix, rotationMatrix), viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), fogStart, fogEnd, clipPlane, textureTranslation);

//	m_Plane->Render(m_D3D->GetDeviceContext());

//	XMMATRIX translateMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f);
//	m_ReflectionShader->Render(m_D3D->GetDeviceContext(), m_Plane->GetIndexCount(), XMMatrixMultiply(worldMatrix, translateMatrix), viewMatrix, projectionMatrix, m_Plane->GetTexture()[1], m_RenderTexture->GetShaderResourceView(), reflectionMatrix);

	return true;

}