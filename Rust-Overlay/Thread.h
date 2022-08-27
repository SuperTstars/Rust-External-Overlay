#pragma once
#include <Windows.h>
#include "RustData.h"
#include "AimBot.h"
namespace Thread {
	static DWORD64 held = NULL;
	VOID GetData();
	VOID GetMatrix();
	VOID DrawEntity();
	VOID AimBotthread();
	VOID NoRecoilthread();
	VOID OpenAdmin();
}
