#include "SystemClass.h"


SystemClass::SystemClass()
{
// object pointer들을 null로 초기화, 이렇게 하지 않으면 Shutdown 함수에서 이들을 CleanUp 하려고 시도할것이기 때문.
// 항상 포인터를 0으로 초기화 해주는 습관을 들이는 것이 좋음
	m_Input = 0;
	m_Graphics = 0;

	m_Fps = 0;
	m_Cpu = 0;
	m_Timer = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{

}
// 소멸자가 아닌 ShutDown 함수에서 모든 object들을 cleanUp 할 것임. 반드시 호출될 것이라는 보장이 없기 때문.
SystemClass::~SystemClass()
{

}


bool SystemClass::Initialize()
{


	int screenWidth, screenHeight;
	bool result;

	// 함수로 보내기 전에 width 와 height를 초기화
	screenWidth = 0;
	screenHeight = 0;

	// windows api를 초기화
	InitializeWindows(screenWidth, screenHeight);

	// Input Object를 생성. 류저로부터 키보드 입력을 받는 데 사용
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// input class 초기화
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Graphics object 생성. 어플리케이션에서 그래픽을 렌더링하는 것을 담당한다.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// Graphics object를 초기화
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	// FpsCLass 생성 및 초기화
	// Create the fps object.
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// CpuClass 생성 및 초기화
	m_Cpu = new CpuClass;
	if (!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	return true;

}

// Graphics object와 input object에 관련된 모든것을 shutdown하고 해제. 윈도우를 닫고 그에 관련된 핸들을 clean up
void SystemClass::Shutdown()
{
	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the cpu object.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Graphics object를 Release
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// input Object 를 Release
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// windows를 shutdown
	ShutdownWindows();

	return;
}

// While not done, check for windows system messages, process system messages, process application loop, check if 사용자가 quit하기를 원하는지
void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// message structure 초기화
	ZeroMemory(&msg, sizeof(MSG));

	// windows로부터, 또는 사용자로부터 quit 메시지가 있을때까지 계속 루프
	done = false;
	while (!done)
	{
		// windows messages handle
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// windows 신호가 어플리케이션의 end를 지시할 경우, exit
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		// 그렇지 않을 경우, frame 처리를 한다.
		else
		{
			result = Frame();
			if (!result)
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
		}

		// Check if the user pressed escape and wants to quit.
		if (m_Input->IsEscapePressed() == true)
		{
			done = true;
		}

	}

	return;

}

// 유저가 esc를 눌렀는지 input object를 체크. 누르지 않았으면 graphics object에서 frame 처리 수행 = 해당 프레임 렌더
// 여기에 그래픽 관련 코드를 추가해 나갈 것임. 
bool SystemClass::Frame()
{

	bool result;
	int mouseX, mouseY;

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// input device의 업데이트 상황을 읽어들였다면, GraphicsCLass의 마우스 위치를 업데이트해서 스크린에 렌더할 수 있게 한다.
	// Get the location of the mouse from the input object,
	m_Input->GetMouseLocation(mouseX, mouseY);



	// graphics ohject에 대해서 frame 처리 수행
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	// Finally render the graphics to the screen.
	result = m_Graphics->Render();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// 이 오브젝트의 외부 포인터를 얻는다.
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스를 얻는다.
	m_hinstance = GetModuleHandle(NULL);

	// 어플리케이션에 이름을 붙인다.
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
