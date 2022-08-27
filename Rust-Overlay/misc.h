#pragma once
#pragma once
#include <Windows.h>
#include <string>
#include <Shlwapi.h>
#include <tchar.h>
#include <fstream>
#include <wininet.h>
#include "Struct.h"
#pragma comment(lib,"Shlwapi.lib")
#pragma comment (lib, "wininet.lib")
#pragma warning(disable : 4996)
#define M_PI 3.14159265358979323846f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
template <typename T> class Singleton
{
protected:
	Singleton() {}
	~Singleton() {}
public:
	static T& get()
	{
		static T instance{};
		return instance;
	}
};
template <typename T> class ObjectMode
{
protected:
	ObjectMode() {}
	~ObjectMode() {}
public:
	static T& getobject()
	{
		static T instance{};
		return instance;
	}
};
class miscs : public Singleton<miscs> {
public:
	std::string AnisToUTF8(const std::string& Str);
	VOID UnicodeToAnsi(const WCHAR* pSrc, char* pDes);
	const char* GetActiveWeapon(const char* name);
	BOOL CheckFullscreen(HWND hWnd);
	BOOL CreateWin10Notify();
	BOOL SendNotify(const char* Title, const char* Text, bool Success);
	BOOL DeleteNotify();
	VOID Sleep_(INT interval);
	std::string GetProgramPath();
	LPSTR strcut(LPSTR old, LPCSTR start, LPCSTR end);
	BOOL NetworkVerification(DWORD PenguinID, LPCSTR Penguin);
	PVOID ReadFileToMemory(LPCSTR fileName, PULONG fileSize);
	POINT GetWindowPos(HWND hwnd);
	POINT GetMousePos(HWND hwnd);
	BOOL KeyState(DWORD Key);
public:
	bool exists(const std::string& name);
	const char* ReadIni_char(LPCSTR path, LPCSTR title, LPCSTR key);
	int ReadIni_int(LPCSTR path, LPCSTR title, LPCSTR key);
	float ReadIni_float(LPCSTR path, LPCSTR title, LPCSTR key);
	bool ReadIni_bool(LPCSTR path, LPCSTR title, LPCSTR key);
	bool WriteIni_char(LPCSTR path, LPCSTR title, LPCSTR key, LPCSTR value);
	bool WriteIni_int(LPCSTR path, LPCSTR title, LPCSTR key, int value);
	bool WriteIni_float(LPCSTR path, LPCSTR title, LPCSTR key, float value);
	bool WriteIni_bool(LPCSTR path, LPCSTR title, LPCSTR key, bool value);
private:
	NOTIFYICONDATAA Data = { 0 };
	bool IsDelete = false;
};