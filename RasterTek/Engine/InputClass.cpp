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

	// 모든 키가 안 눌린 상태인 것으로
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	// key가 눌렸을 경우, 해당 키의 상태를 배열에 저장
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

