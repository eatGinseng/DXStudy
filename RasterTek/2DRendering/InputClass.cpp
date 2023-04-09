#include "InputClass.h"

InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass& other)
{


}

InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
	int i;

	// ��� Ű�� �� ���� ������ ������
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	// key�� ������ ���, �ش� Ű�� ���¸� �迭�� ����
	m_keys[input] = true;
	return;

}

void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}

