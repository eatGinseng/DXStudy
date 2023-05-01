////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;

	m_Text = 0;

	m_LightShader = 0;
	m_Light = 0;

	m_ModelList = 0;

	m_Frustum = 0;

	m_Cursor = 0;
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

	// Create the model object
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	char textureFilename[128];
	strcpy_s(textureFilename, "seafloor.tga");

	char modelFilename[128];
	ZeroMemory(modelFilename, sizeof(char) * 128);
	strcpy_s(modelFilename, "sphere.txt");

	// Initialize the model object
	result = m_Model->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), textureFilename, hwnd, modelFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Light Shader class 초기화
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(3.0f);

	// Create the model list object.
	m_ModelList = new ModelListClass;
	if (!m_ModelList)
	{
		return false;
	}

	// Initialize the model list object.
	result = m_ModelList->Initialize(25);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}
	
	// Create Frustum class object
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
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

	result = m_Cursor->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), screenWidth, screenHeight, 50, 50, "Cursor.tga", hwnd, baseViewMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the model list object.
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}


	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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


bool GraphicsClass::Frame(int fps, int cpu, float frameTime, int mouseX, int mouseY, float rotationY)
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

	// Set the rotation of the camera.
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	return true;
}


bool GraphicsClass::Render()
{
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	XMVECTOR color;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool renderModel, result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// construct the frustum
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// 렌더할 모델 갯수를 얻는다.
	modelCount = m_ModelList->GetModelCount();

	// 렌더링 될 모델 갯수 초기화
	renderCount = 0;

	// ModelListClass 객체의 모든 모델을 돈다.
	for (index = 0; index < modelCount; index++)
	{
		// 해당 인덱스의 모델의 위치와 컬러를 얻는다.
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// radius는 1.0으로 정한다.
		radius = 1.0f;

		// 여기에서 frustum 으로 sphere가 보이는지 안보이는지 체크한다.
		// sphere model이 view frustum 안에 있으면 렌더하고, 안보이면 스킵한다.
		renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);

		// 보이면 렌더, 안보이면 스킵하고 다음 모델
		if (renderModel)
		{
			// 렌더링되어야 할 위치로 모델을 옮긴다.
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// model vertex와 index buffer를 그래픽스 파이프라인에 넣는다. 
			m_Model->Render(m_D3D->GetDeviceContext());

			// light shader로 모델을 렌더링한다.
			m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), color, m_Light->GetAmbientColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			
			// Reset to original matrix
			m_D3D->GetWorldMatrix(worldMatrix);

			// 모델이 렌더링 됐기 때문에 갯수도 업데이트 해 준다.
			renderCount++;

		}

	}

	// text에 model 갯수 전달
	result = m_Text->SetRenderCount(renderCount, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

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