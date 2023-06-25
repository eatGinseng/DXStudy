#include <d3d11.h>
#include "main.h"
#include "SystemClass.h"


// ---------- 선언 -------------------
#define WIN_WIDTH	800
#define WIN_HEIGHT	600

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {

	SystemClass* System;
	bool result;

	// System object 만들기
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// System object 초기화하고 실행하기
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// System object 종료 및 해제
	System->Shutdown();
	delete System;
	System = 0;

	return 0;

};