#include "Includes.h"
#include "renderer.h"
#include "Process.h"
#include "RustData.h"
#include "Thread.h"
VOID Star() {
	Draw::get().DrawNewMenu();
	Thread::DrawEntity();
}
BOOL WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT nCmdShow)
//BOOL main()
{
	//setting();
	ULONG FileSize;
	miscs::get().CreateWin10Notify();
	BOOL sucessful = Process::get().attach(L"RustClient.exe");
	if (!sucessful) 
	{
		miscs::get().SendNotify("Error", "请打开游戏后用管理员权限运行!", sucessful);
		miscs::get().DeleteNotify();
		return FALSE;
	}
	Entity::getobject().GameAssembly = Process::get().GetModuleAddress(L"GameAssembly.dll");
	if (Entity::getobject().GameAssembly >= 0x700000000000 && Entity::getobject().GameAssembly <= 0x7FFFFFFFFFFF)
		miscs::get().SendNotify("Welcome", "欢迎使用~", TRUE);
	else {
		miscs::get().SendNotify("Error", "无法获取游戏数据!", FALSE);
		miscs::get().DeleteNotify();
		return FALSE;
	}
	Setting::get().ReadRendererSettings();
	Setting::get().ReadColorSettings();
	Setting::get().ReadDistanceSettings();
	std::thread data(Thread::GetData);
	data.detach();
	std::thread matrix(Thread::GetMatrix);
	matrix.detach();
	std::thread aimbot(Thread::AimBotthread); 
	aimbot.detach();
	std::thread Recoil(Thread::NoRecoilthread);
	Recoil.detach();
	std::thread admin(Thread::OpenAdmin);
	admin.detach();
	PVOID FileBuffer = miscs::get().ReadFileToMemory((miscs::get().GetProgramPath() + "\\Font.dll").c_str(), &FileSize);
	Draw::get().Install(L"Rust", L"UnityWndClass", FileBuffer, FileSize);
	Draw::get().Create(Star);
	return TRUE;
}