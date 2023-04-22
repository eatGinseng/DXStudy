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

	// ���콺 Ŀ���� ��ġ��ų �� ����� ȭ�� ���μ��� ����� ��´�.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// ���콺 ��ġ �ʱ�ȭ
	m_mouseX = 0;
	m_mouseY = 0;

	// DirectInput�� �������̽��� �ʱ�ȭ�Ѵ�. �������̽��� �ʱ�ȭ �ϸ�, �ٸ� ��ǲ ����̽��� �ʱ�ȭ �� �� �ִ�.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Ű���带 ���� direct input interface�� �ʱ�ȭ
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ������ ������ ���Ѵ�. Ű�����̱� ������, �̸� ���ǵ� ������ ����� �� �ִ�.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Cooperative Level�� ���ø����̼��� �������� ��, Ű���� �Է��� �ٸ� ���α׷��� ������ �� ���θ� ����
	// �������� �������� DISCL_EXCLUSIVE, �����Ϸ��� non-exclusive (DISCL_NONEXCLUSIVE)
	result - m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Ű���带 ��´�.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// ���콺 �¾�
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

	// Ű���� ���� �б�
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// ���콺 ���� �б�
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// ���콺�� Ű������ ��ǲ process
	ProcessInput();

	return true;
}

// ReadKeyboard()������ Ű���� ���¸� m_keyboardState�� �о���δ�. state�� Ű������ � Ű�� ���� ���ȴ��� �ȴ��ȴ��� �����ش�.
// ���� Ű���带 �о������ ���ߴٸ�, �� ��쿡�� �ٽ� ��Ʈ���� ���� ������ �� ������ acquire()�� ȣ���Ѵ�.
// ��Ŀ���� �Ҿ��� ��, �Ǵ� �ּ�ȭ ���� �� acquire�� ������ �� ������, �ٽ� foreground�� ���� �� �ٽ� Ű���� state�� ���� �� �ִ�.
bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// Ű���� ����̽� �б�
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// Keyboard �� focus�� �Ұų� acquire���� ���ߴٸ� ��Ʈ���� �ٽ� ��ã�� �õ�
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

// ���콺 �Է��� �޾ƿ��� �� Ű����� ���� ����ϳ�, ���� ���������κ��� �󸶳� ���������� ������� ��ġ �������� �о����
bool InputClass::ReadMouse()
{
	HRESULT result;

	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// ���콺 focus�� �Ҿ���ȴٸ�, �ٽ� ��ã�� �õ�
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

// processInput �Լ�������, ���� ���������κ��� ��ǲ ����̽����� ��ȭ�� �ٷ��.
// �� Ʃ�丮�󿡼��� �����ϰ� ���콺 ��ġ�� ������Ʈ�� ���̴�. (��������� ���콺 Ŀ�� ��ġ�� �����ϴ� �Ͱ� �ٽ��ϰ�)
// 0���� �ʱ�ȭ�� m_mouseX�� m_mouseY ������ ���콺�� ��ȭ�� ��ġ�� add �� �� ���̴�.
// ���콺�� ȭ�� �ٱ����� ����� �ʾҴ��� üũ�ϴ� �Ϳ� ��������. 
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

// Ư�� Ű�� ���ȴ��� üũ�ϴ� �Լ�. �ʿ信 ���� �ٸ� Ű�� ���ؼ��� �߰��� �� �ִ�.
bool InputClass::IsEscapePressed()
{
	// bitwise �� escape Ű�� ���ȴ� �� üũ�Ѵ�.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}


// ���� GraphicClass���� �� ������ �����.
void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}


