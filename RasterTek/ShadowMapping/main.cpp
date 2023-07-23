#include <d3d11.h>
#include "main.h"
#include "SystemClass.h"


// ---------- ���� -------------------
#define WIN_WIDTH	800
#define WIN_HEIGHT	600

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {

	SystemClass* System;
	bool result;

	// System object �����
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// System object �ʱ�ȭ�ϰ� �����ϱ�
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// System object ���� �� ����
	System->Shutdown();
	delete System;
	System = 0;

	return 0;

};