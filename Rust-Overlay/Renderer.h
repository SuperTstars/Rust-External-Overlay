#pragma once
#include "Includes.h"
#include "Struct.h"
#include "misc.h"
typedef VOID(*_DrawFunc)();
class Draw : public Singleton<Draw>
{
public:
	BOOL Install(LPCWSTR WindowName, LPCWSTR WindowClassName, PVOID FontBuffer,ULONG BufferSize, FLOAT FontSize = 15.0f);
	VOID Create(_DrawFunc Drawfunc);
	VOID DrawString(FLOAT x, FLOAT y, LPCSTR String, ImVec4 Colour, BOOL OutLine);
	VOID DrawCircleFilled(FLOAT x, FLOAT y, FLOAT Radius, ImVec4 Colour);
	VOID DrawCircle(FLOAT x, FLOAT y, FLOAT Radius, ImVec4 Colour, FLOAT thickness);
	VOID DrawRect(FLOAT x, FLOAT y, FLOAT w, FLOAT h, ImVec4 Colour, FLOAT thickness);
	VOID DrawRectFilled(FLOAT x, FLOAT y, FLOAT w, FLOAT h, ImVec4 Colour);
	VOID DrawLine(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, ImVec4 Colour, FLOAT thickness);
	VOID DrawWindows(BOOL v, LPCSTR Title);
	VOID DrawButton(FLOAT x, FLOAT y, LPCSTR String, BOOL v);
	VOID DrawChoose(FLOAT x, FLOAT y, LPCSTR String, BOOL v);
	VOID Key();
	VOID RangeFloat(FLOAT x, FLOAT y, LPCSTR String);
	VOID DrawNewMenu();
public:
	ImFont* Font = new ImFont,*MenuFont = new ImFont;
	INT WindowWidth = NULL, WindowHeight = NULL;
	HWND GameHwnd = NULL;
	INT SleepTime = 5;
	BOOL ESP = TRUE;
	BOOL _Aimbot = FALSE;
	BOOL Memory = FALSE;
	BOOL Rander = FALSE;
	FLOAT _x = 115, _y = 63;
private:
	HWND TopHwnd = NULL;
	PVOID Buffer = NULL;
	FLOAT Size = NULL;
	Vec2 Local{};
	Vec2 mousepos{};
};
ImVec4 RGBA(FLOAT R, FLOAT G, FLOAT B, FLOAT A);