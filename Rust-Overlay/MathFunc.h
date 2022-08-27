#pragma once
#include "Includes.h"
#include "process.h"
#include "Struct.h"
#include "Renderer.h"
class Algorithm : public Singleton<Algorithm> {
public:
	FLOAT Get2Ddistance(Vec2 pos1, Vec2 pos2);
	FLOAT Get3Ddistance(Vec3 pos1, Vec3 pos2);
	VOID ReadMatrix(DWORD64 MatrixAddress, FLOAT Matrix[4][4]);
	BOOL WorldToScreen_AimBot(FLOAT Matrix[4][4], Vec3 Object, Vec2& ScreenPiont, INT WindowWindth, INT WindowHeigh);
	BOOL WorldToScreen(FLOAT Matrix[4][4], Vec3 Object, Vec2& ScreenPiont, INT WindowWindth, INT WindowHeigh);
	BOOL WorldToScreen(FLOAT Matrix[4][4], Vec3 Object, Vec2& ScreenPiont, Vec2& ScreenText, INT WindowWindth, INT WindowHeigh);
	BOOL WorldToScreen(FLOAT Matrix[4][4], Vec3 Object, Vec4& ScreenRect,Vec2& ScreenPiont, INT WindowWindth, INT WindowHeigh);
	DOUBLE Torad(DOUBLE degree);
	Vec2 RadarCalc(Vec3 MyPos, Vec3 Pos, INT size, FLOAT AngleX);
	DOUBLE GetX(DOUBLE x, DOUBLE y);
	DOUBLE GetY(DOUBLE x, DOUBLE y, DOUBLE z);
	Vec4 Transformation(Vec3 ev);
};