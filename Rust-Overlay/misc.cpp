#include "misc.h"
std::string miscs::AnisToUTF8(const std::string& Str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, Str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[(size_t)nwLen + 1];
	ZeroMemory(pwBuf, (size_t)nwLen * 2 + 2);
	::MultiByteToWideChar(CP_ACP, 0, Str.c_str(), Str.length(), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[(size_t)nLen + 1];
	ZeroMemory(pBuf, (size_t)nLen + 1);
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr(pBuf);
	delete[]pwBuf;
	delete[]pBuf;
	pwBuf = NULL;
	pBuf = NULL;
	return retStr;
}
VOID miscs::UnicodeToAnsi(const WCHAR* pSrc, char* pDes)
{
	char defaultChar[100] = { 0 };
	BOOL bUseDefaultChar = FALSE;
	WideCharToMultiByte(CP_ACP, 0, pSrc, -1, pDes, 1024, defaultChar, &bUseDefaultChar);
}
BOOL miscs::CheckFullscreen(HWND hWnd)
{
	bool bFullScreen = false;
	RECT rcApp, rcDesk;
	GetWindowRect(GetDesktopWindow(), &rcDesk);
	if (hWnd != GetDesktopWindow() && hWnd != GetShellWindow()) {
		GetWindowRect(hWnd, &rcApp);
		if (rcApp.left <= rcDesk.left && rcApp.top <= rcDesk.top && rcApp.right >= rcDesk.right && rcApp.bottom >= rcDesk.bottom) {
			bFullScreen = true;
		}
	}
	return bFullScreen;
}
const char* miscs::GetActiveWeapon(const char* name) {
	if (strstr(name, "Assault"))
		return "AK-47ͻ����ǹ";
	if (strstr(name, "LR-300"))
		return "LR-300ͻ����ǹ";
	if (strstr(name, "L96"))
		return "L96�ѻ���ǹ";
	if (strstr(name, "Medical"))
		return "ҽ��ע����";
	if (strstr(name, "Thompson"))
		return "����ɭ���ǹ";
	if (strstr(name, "M39"))
		return "M39��ǹ";
	if (strstr(name, "M249"))
		return "����";
	if (strstr(name, "MP5A4"))
		return "MP5���ǹ";
	if (strstr(name, "M92"))
		return "M92��ǹ";
	if (strstr(name, "Spas-12"))
		return "��������ǹ";
	if (strstr(name, "Semi-Automatic Rifle"))
		return "���Զ���ǹ";
	if (strstr(name, "Crossbow"))
		return "��";
	if (strstr(name, "Wooden Spear"))
		return "ľì";
	if (strstr(name, "Bolt Action"))
		return "���";
	if (strstr(name, "F1 Grenade"))
		return "F1����";
	if (strstr(name, "Double Barrel"))
		return "�����ͽ";
	if (strstr(name, "Compound"))
		return "���Ϲ�";
	if (strstr(name, "Machete"))
		return "�󿳵�";
	if (strstr(name, "Salvaged Axe"))
		return "Բ��";
	if (strstr(name, "Eoka"))
		return "С����";
	if (strstr(name, "Combat"))
		return "ذ��";
	if (strstr(name, "Paddle"))
		return "��";
	if (strstr(name, "Pump"))
		return "�ö�ɢ��ǹ";
	if (strstr(name, "Python Revolve"))
		return "������";
	if (strstr(name, "Revolver"))
		return "С����";
	if (strstr(name, "Waterpipe"))
		return "������";
	if (strstr(name, "Salvaged lcepick"))
		return "����ʮ�ָ�";
	if (strstr(name, "Salvaged Cleaver"))
		return "�������⵶";
	if (strstr(name, "Salvaged Sword"))
		return "���Ͻ�";
	if (strstr(name, "Semi-Automatic Pisto"))
		return "���Զ���ǹ";
	if (strstr(name, "Hammer"))
		return "Сľ��";
	if (strstr(name, "Bone Knife"))
		return "����";
	if (strstr(name, "Bone Club"))
		return "�ǰ�";
	if (strstr(name, "Logsword"))
		return "����";
	if (strstr(name, "Nailgun"))
		return "��ǹ";
	if (strstr(name, "Beancan Grenade"))
		return "������";
	if (strstr(name, "Custom SMG"))
		return "С�۷���ǹ";
	if (strstr(name, "Rocket Launcher"))
		return "���Ͳ";
	if (strstr(name, "Hunting Bow"))
		return "�Թ�";
	if (strstr(name, "Stone Spear"))
		return "ʯì";
	if (strstr(name, "Mace"))
		return "������";
	if (strstr(name, "Flame Thrower"))
		return "���淢����";
	if (strstr(name, "Code Lock"))
		return "������";
	if (strstr(name, "Key Lock"))
		return "Կ����";
	if (strstr(name, "Tool Cupboard"))
		return "��ع�";
	if (strstr(name, "Building Plan"))
		return "����ͼֽ";
	if (strstr(name, "Wooden Ladder"))
		return "����";
	if (strstr(name, "Workbench Level 1"))
		return "һ������̨";
	if (strstr(name, "Workbench Level 2"))
		return "��������̨";
	if (strstr(name, "Workbench Level 3"))
		return "��������̨";
	if (strstr(name, "Red Keycard"))
		return "�쿨";
	if (strstr(name, "Blue Keycard"))
		return "����";
	if (strstr(name, "Green Keycard"))
		return "�̿�";
	if (strstr(name, "Flashlight"))
		return "�ֵ�Ͳ";
	if (strstr(name, "Bandage"))
		return "����";
	if (strstr(name, "Sgotgun Trap"))
		return "ɢ��ǹ����";
	if (strstr(name, "Satchel Charge"))
		return "����ըҩ��";
	if (strstr(name, "Sleeping Bag"))
		return "˯��";
	if (strstr(name, "Torch"))
		return "���";
	if (strstr(name, "hansaw"))
		return "���";
	if (strstr(name, "ackhammer"))
		return "���";
	if (strstr(name, "Furnace"))
		return "��¯";
	if (strstr(name, "Stone Hatch"))
		return "ʯ��";
	if (strstr(name, "Stone Picka"))
		return "ʯ��";
	if (strstr(name, "Hatchet"))
		return "����";
	if (strstr(name, "Pickaxe"))
		return "����";
	if (strstr(name, "multiplegrena"))
		return "�񵯷�����";
	if (strstr(name, "Timed"))
		return "C4ը��";
	if (strstr(name, "Blueberries"))
		return "��ݮ";
	return "";
}
BOOL miscs::CreateWin10Notify()
{
	Data.cbSize = sizeof(NOTIFYICONDATAA);
	Data.hWnd = GetConsoleWindow();
	Data.uID = 1;
	Data.uFlags = NIF_TIP | NIF_ICON;
	Data.hIcon = (HICON)SendMessageA(GetConsoleWindow(), 127, 0, 0);
	strcpy_s(Data.szTip, "TongL");
	return Shell_NotifyIconA(NIM_ADD, &Data);
}
BOOL miscs::SendNotify(const char* Title, const char* Text, bool Success)
{
	Data.uFlags = NIF_INFO;
	if (Success) Data.dwInfoFlags = NIIF_INFO;
	else Data.dwInfoFlags = NIIF_ERROR;
	strcpy_s(Data.szInfo, Text);
	strcpy_s(Data.szInfoTitle, Title);
	return Shell_NotifyIconA(NIM_MODIFY, &Data);
}
BOOL miscs::DeleteNotify()
{
	return Shell_NotifyIconA(NIM_DELETE, &Data);
}
VOID miscs::Sleep_(INT interval) {
	HANDLE hTimer;
	LARGE_INTEGER int64;
	MSG msg;
	int64.QuadPart = -10 * interval;
	hTimer = CreateWaitableTimer(0, false, 0);
	SetWaitableTimer(hTimer, &int64, 0, 0, 0, false);
	while (MsgWaitForMultipleObjects(1, &hTimer, false, -1, 255) != 0) {
		while (PeekMessageW(&msg, 0, 0, 0, 1) != 0) {
			DispatchMessage(&msg);
			TranslateMessage(&msg);
		}
	}
	CloseHandle(hTimer);
}
std::string miscs::GetProgramPath()
{
	CHAR exeFullPath[MAX_PATH];
	std::string Path = "";
	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	PathRemoveFileSpecA(exeFullPath);
	Path = Path + exeFullPath;
	return Path;
}
const char* miscs::ReadIni_char(LPCSTR path, LPCSTR title, LPCSTR key) {
	char str[256] = { 0 };
	GetPrivateProfileStringA(title, key, "", str, 256, path);
	return str;
}
int miscs::ReadIni_int(LPCSTR path, LPCSTR title, LPCSTR key) {
	return GetPrivateProfileIntA(title, key, 0, path);
}
float miscs::ReadIni_float(LPCSTR path, LPCSTR title, LPCSTR key) {
	char str[256] = { 0 };
	GetPrivateProfileStringA(title, key, "", str, 256, path);
	return (float)atof(str);
}
bool miscs::ReadIni_bool(LPCSTR path, LPCSTR title, LPCSTR key) {
	return (bool)GetPrivateProfileIntA(title, key, 0, path);
}
bool miscs::WriteIni_char(LPCSTR path, LPCSTR title, LPCSTR key, LPCSTR value) {
	return WritePrivateProfileStringA(title, key, value, path);
}
bool miscs::WriteIni_int(LPCSTR path, LPCSTR title, LPCSTR key, int value) {
	char str[256] = { 0 };
	sprintf(str, "%d", value);
	return WritePrivateProfileStringA(title, key, str, path);
}
bool miscs::WriteIni_float(LPCSTR path, LPCSTR title, LPCSTR key, float value) {
	char str[256] = { 0 };
	sprintf(str, "%f", value);
	return WritePrivateProfileStringA(title, key, str, path);
}
bool miscs::WriteIni_bool(LPCSTR path, LPCSTR title, LPCSTR key, bool value) {
	char str[256] = { 0 };
	sprintf(str, "%d", value);
	return WritePrivateProfileStringA(title, key, str, path);
}
bool miscs::exists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}
LPSTR miscs::strcut(LPSTR old, LPCSTR start, LPCSTR end)
{
	CHAR str[256] = { 0 };
	LPSTR strtmp1 = strstr(old, start);
	LPSTR strtmp2 = strstr(old, end);
	LPSTR newstr = new char[strtmp2 - strtmp1];
	memcpy(str, strncpy(newstr, strtmp1, strtmp2 - strtmp1) + 3, strtmp2 - strtmp1);
	return str;
}
BOOL miscs::NetworkVerification(DWORD PenguinID, LPCSTR Penguin) {
	CHAR Urltext[256] = { 0 };
	ULONG Number = NULL;
	BOOL ret = FALSE;
	std::string url = "https://r.qzone.qq.com/fcg-bin/cgi_get_portrait.fcg?uins=" + std::to_string(PenguinID);
	HINTERNET Session = InternetOpenA("RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!Session) return FALSE;
	HINTERNET Handle = InternetOpenUrlA(Session, url.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (!Handle) return FALSE;
	InternetReadFile(Handle, Urltext, 255, &Number);
	InternetCloseHandle(Handle);
	Handle = NULL;
	InternetCloseHandle(Session);
	Session = NULL;
	if (!Urltext) return FALSE;
	if (!strcmp(Penguin, strcut(Urltext, "0,\"", "\",0"))) ret = TRUE; else ret = FALSE;
	return ret;
}
PVOID miscs::ReadFileToMemory(LPCSTR fileName, PULONG fileSize)
{
	HANDLE fileHandle = NULL;
	DWORD readd = 0;
	PVOID fileBufPtr = NULL;
	fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		*fileSize = 0;
		return NULL;
	}
	*fileSize = GetFileSize(fileHandle, NULL);
	fileBufPtr = calloc(1, *fileSize);
	if (!ReadFile(fileHandle, fileBufPtr, *fileSize, &readd, NULL))
	{
		free(fileBufPtr);
		fileBufPtr = NULL;
		*fileSize = 0;
	}
	CloseHandle(fileHandle);
	return fileBufPtr;
}
POINT miscs::GetWindowPos(HWND hwnd) {
	POINT v{};
	v.x = 0;
	v.y = 0;
	ClientToScreen(hwnd,&v);
	return v;
}
POINT miscs::GetMousePos(HWND hwnd) {
	POINT v1{}, v2{};
	v2 = this->GetWindowPos(hwnd);
	GetCursorPos(&v1);
	v1.x = v1.x - v2.x;
	v1.y = v1.y - v2.y;
	return v1;
}
BOOL miscs::KeyState(DWORD Key) {
	return (BYTE)GetKeyState(Key) >= 128;
}