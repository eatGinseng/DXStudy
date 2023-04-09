#include "SystemClass.h"


SystemClass::SystemClass()
{
// object pointer���� null�� �ʱ�ȭ, �̷��� ���� ������ Shutdown �Լ����� �̵��� CleanUp �Ϸ��� �õ��Ұ��̱� ����.
// �׻� �����͸� 0���� �ʱ�ȭ ���ִ� ������ ���̴� ���� ����
m_Input = 0;
m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{

}
// �Ҹ��ڰ� �ƴ� ShutDown �Լ����� ��� object���� cleanUp �� ����. �ݵ�� ȣ��� ���̶�� ������ ���� ����.
SystemClass::~SystemClass()
{

}


bool SystemClass::Initialize()
{

	int screenWidth, screenHeight;
	bool result;

	// �Լ��� ������ ���� width �� height�� �ʱ�ȭ
	screenWidth = 0;
	screenHeight = 0;

	// windows api�� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	// Input Object�� ����. �����κ��� Ű���� �Է��� �޴� �� ���
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// inptu object�� �ʱ�ȭ
	m_Input->Initialize();

	// Graphics object ����. ���ø����̼ǿ��� �׷����� �������ϴ� ���� ����Ѵ�.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// Graphics object�� �ʱ�ȭ
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;

}

// Graphics object�� input object�� ���õ� ������ shutdown�ϰ� ����. �����츦 �ݰ� �׿� ���õ� �ڵ��� clean up
void SystemClass::Shutdown()
{
	// Graphics object�� Release
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// input Object �� Release
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// windows�� shutdown
	ShutdownWindows();

	return;
}

// While not done, check for windows system messages, process system messages, process application loop, check if ����ڰ� quit�ϱ⸦ ���ϴ���
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// message structure �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));

	// windows�κ���, �Ǵ� ����ڷκ��� quit �޽����� ���������� ��� ����
	done = false;
	while (!done)
	{
		// windows messages handle
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// windows ��ȣ�� ���ø����̼��� end�� ������ ���, exit
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		// �׷��� ���� ���, frame ó���� �Ѵ�.
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;

}

// ������ esc�� �������� input object�� üũ. ������ �ʾ����� graphics object���� frame ó�� ���� = �ش� ������ ����
// ���⿡ �׷��� ���� �ڵ带 �߰��� ���� ����. 
bool SystemClass::Frame()
{

	bool result;

	// ����ڰ� esc�� ������, ���ø����̼� ���Ḧ ���ϴ���?
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// graphics ohject�� ���ؼ� frame ó�� ����
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	// key �� ���ȴ��� üũ
	case WM_KEYDOWN:
	{
		// input object�� ���� Ű�� ������ ���¸� ������ �� �ֵ��� �Ѵ�.
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	// key �� release �Ǿ�����?
	case WM_KEYUP:
	{
		// ���¸� �ٽ� unset�Ҽ��ְ� input object�� ������
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
	
	// �ٸ� �޽������� ���ø����̼ǿ��� ��� ����
	default:
	{

		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	}

}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// �� ������Ʈ�� �ܺ� �����͸� ��´�.
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� ��´�.
	m_hinstance = GetModuleHandle(NULL);

	// ���ø����̼ǿ� �̸��� ���δ�.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
	posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
