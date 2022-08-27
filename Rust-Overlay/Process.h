#pragma once
#include "Includes.h"
#include "misc.h"
#define READ				0x10000
#define WRITE				0x20000
#define READMDL				0x30000
#define WRITEMDL			0x40000
#define READPHYSICAL		0x50000
#define WRITEPHYSICAL		0x60000
#define GETMODULE			0x70000
#define AllOCATE			0x80000
#define FREE				0x90000
#define INJECTx32			0x100000
#define INJECTx64			0x110000
#define INSTALLINJECT		0x120000
#define UNINSTALLINJECT		0x130000
#define HIDEWINDOWS			0x140000
#define REMOTETHREAD		0x150000
#define END					0x160000
typedef struct _Driver {
	DWORD Flags;
	DWORD ProcessID;
	DWORD64 MemoryAddress;
	DWORD64 Buffer;
	DWORD BufferSize;
	DWORD64 ModuleName;
	DWORD AllocType;
	DWORD FreeType;
	DWORD64 DllBuffer;
	DWORD Hwnd;
	DWORD64 ThreadAddress;
	DWORD64 ParameterAddress;
	DWORD ThreadFlags;
	DWORD ThreadWait;
	DWORD64 ThreadStatus;
} Driver, * PDriver;
enum class InjectType
{
	x32 = 1,
	x64 = 2
};
class Process : public Singleton<Process> {
public:
	/*安装驱动*/
	BOOL Install();
	/*卸载驱动*/
	BOOL UnInstall();
	/*安装全局注入*/
	BOOL InstallInject();
	/*卸载全局注入*/
	BOOL UnInstallInject();
	/*附加一个进程*/
	BOOL attach(LPCWSTR ProcessName);
	/*反截图*/
	BOOL HideWindows(DWORD hwnd);
	/*通过进程名获取进程ID*/
	DWORD GetProcessId(LPCWSTR ProcessName);
	/*获取模块基址*/
	DWORD64 GetModuleAddress(LPCWSTR ModuleName);
	/*申请内存*/
	DWORD64 AllocMemory(DWORD size, DWORD AllocType);
	/*释放内存*/
	BOOL FreeMemory(DWORD64 address, DWORD size, DWORD FreeType);
	/*全局注入*/
	BOOL Inject(PBYTE DllBuffer, DWORD DllSize, InjectType InjectType);
	/*创建远程线程*/
	DWORD KernelCreateRemoteThread(DWORD64 ThreadAddress, DWORD64 ParameterAddress, DWORD ThreadFlags, BOOL ThreadWait);
	/*读*/
	template <typename T> BOOL Read(DWORD64 address, T* buffer, DWORD size);
	/*写*/
	template <typename T> BOOL Write(DWORD64 address, T* buffer, DWORD size);
public:
	DWORD pid = NULL;
	BOOL IsLoad();
	BOOL ReadMemory(DWORD64 address, PBYTE Buffer, DWORD size);
	BOOL WriteMemory(DWORD64 address, PBYTE Buffer, DWORD size);
};
template <typename T> BOOL Process::Read(DWORD64 address, T* buffer, DWORD size) {
	return ReadMemory(address, (PBYTE)buffer, size);
}
template <typename T> BOOL Process::Write(DWORD64 address, T* buffer, DWORD size) {
	return WriteMemory(address, (PBYTE)buffer, size);
}