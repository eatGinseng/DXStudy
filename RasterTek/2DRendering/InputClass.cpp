#include "InputClass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}

InputClass::InputClass(const InputClass& other)
{


}

InputClass::~InputClass()
{

}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// 마우스 커서를 위치시킬 때 사용할 화면 가로세로 사이즈를 얻는다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// 마우스 위치 초기화
	m_mouseX = 0;
	m_mouseY = 0;

	// DirectInput의 인터페이스를 초기화한다. 인터페이스를 초기화 하면, 다른 인풋 디바이스도 초기화 할 수 있다.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드를 위한 direct input interface를 초기화
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 포맷을 정한다. 키보드이기 때문에, 미리 정의된 포맷을 사용할 수 있다.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Cooperative Level은 어플리케이션이 실행중일 때, 키보드 입력을 다른 프로그램과 공유할 지 여부를 결정
	// 공유하지 않으려면 DISCL_EXCLUSIVE, 공유하려면 non-exclusive (DISCL_NONEXCLUSIVE)
	result - m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드를 얻는다.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스 셋업
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	// Release the mouse.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	// 키보드 상태 읽기
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// 마우스 상태 읽기
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// 마우스와 키보드의 인풋 process
	ProcessInput();

	return true;
}

// ReadKeyboard()에서는 키보드 상태를 m_keyboardState로 읽어들인다. state는 키보드의 어떤 키가 현재 눌렸는지 안눌렸는지 보여준다.
// 만약 키보드를 읽어들이지 못했다면, 이 경우에는 다시 컨트롤을 얻을 때까지 매 프레임 acquire()를 호출한다.
// 포커스를 잃었을 때, 또는 최소화 했을 때 acquire가 실패할 수 있지만, 다시 foreground로 왔을 때 다시 키보드 state를 얻을 수 있다.
bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// 키보드 디바이스 읽기
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// Keyboard 가 focus를 잃거나 acquire되지 못했다면 컨트롤을 다시 되찾기 시도
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

// 마우스 입력을 받아오는 건 키보드와 거의 비슷하나, 이전 프레임응로부터 얼마나 움직였는지 상대적인 위치 정보만을 읽어들임
bool InputClass::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// 마우스 focus를 잃어버렸다면, 다시 되찾기 시도
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

// processInput 함수에서는, 이전 프레임으로부터 인풋 디바이스들의 변화를 다룬다.
// 이 튜토리얼에서는 간단하게 마우스 위치를 업데이트할 것이다. (윈도우즈에서 마우스 커서 위치를 추적하는 것과 바슷하게)
// 0으로 초기화된 m_mouseX와 m_mouseY 변수에 마우스의 변화된 위치를 add 해 줄 것이다.
// 마우스가 화면 바깥으로 벗어나지 않았는지 체크하는 것에 주의하자. 
void InputClass::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;

}

// 특정 키가 눌렸는지 체크하는 함수. 필요에 따라 다른 키에 대해서도 추가할 수 있다.
bool InputClass::IsEscapePressed()
{
	// bitwise 로 escape 키가 눌렸는 지 체크한다.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}


// 이후 GraphicClass에서 이 정보를 사용함.
void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}


