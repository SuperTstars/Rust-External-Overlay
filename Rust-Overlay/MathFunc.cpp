#include "mathfunc.h"
#include "global.h"

FLOAT Algorithm::Get2Ddistance(Vec2 pos1, Vec2 pos2)
{
	return static_cast<FLOAT>(sqrt(pow(static_cast<DOUBLE>(abs(pos1.x - pos2.x)), 2) + pow(static_cast<DOUBLE>(abs(pos1.y - pos2.y)), 2)));
}
FLOAT Algorithm::Get3Ddistance(Vec3 pos1, Vec3 pos2)
{
	return static_cast<DOUBLE>(sqrt(pow(sqrt(pow(static_cast<DOUBLE>(abs(pos1.x - pos2.x)), 2) + pow(static_cast<DOUBLE>(abs(pos1.y - pos2.y)), 2)), 2) + pow(static_cast<DOUBLE>(abs(pos1.z - pos2.z)), 2)));
}
VOID Algorithm::ReadMatrix(DWORD64 MatrixAddress, FLOAT Matrix[4][4]) {
	Process::get().Read<BYTE>(MatrixAddress, (PBYTE)Matrix, 64);
}
BOOL Algorithm::WorldToScreen_AimBot(FLOAT Matrix[4][4], Vec3 Object, Vec2& ScreenPiont, INT WindowWindth, INT WindowHeigh) {
	FLOAT x, y1, y2, ViewW;
	ViewW = Object.x * Matrix[0][3] + Object.y * Matrix[1][3] + Object.z * Matrix[2][3] + Matrix[3][3];
	if (ViewW < 0.f) return FALSE;
	ViewW = 1.0f / ViewW;
	x = WindowWindth / 2 + (Object.x * Matrix[0][0] + Object.y * Matrix[1][0] + Object.z * Matrix[2][0] + Matrix[3][0]) * ViewW * WindowWindth / 2;
	y1 = WindowHeigh / 2 - (Object.x * Matrix[0][1] + (Object.y + DisPos - Down) * Matrix[1][1] + Object.z * Matrix[2][1] + Matrix[3][1]) * ViewW * WindowHeigh / 2;
	y2 = WindowHeigh / 2 - (Object.x * Matrix[0][1] + (Object.y + 1.2f - Down) * Matrix[1][1] + Object.z * Matrix[2][1] + Matrix[3][1]) * ViewW * WindowHeigh / 2;
	if (Position) ScreenPiont.Set(x, y1);else ScreenPiont.Set(x, y2);
	return TRUE;
}
BOOL Algorithm::WorldToScreen(FLOAT Matrix[4][4], Vec3 Object, Vec2& ScreenPiont, INT WindowWindth, INT WindowHeigh) {
	FLOAT x,y,ViewW;
	ViewW = Object.x * Matrix[0][3] + Object.y * Matrix[1][3] + Object.z * Matrix[2][3] + Matrix[3][3];
	if (ViewW < 0.f) return FALSE;
	ViewW = 1.0f / ViewW;
	x = WindowWindth / 2 + (Object.x * Matrix[0][0] + Object.y * Matrix[1][0] + Object.z * Matrix[2][0] + Matrix[3][0]) * ViewW * WindowWindth / 2;
	y = WindowHeigh / 2 - (Object.x * Matrix[0][1] + Object.y * Matrix[1][1] + Object.z * Matrix[2][1] + Matrix[3][1]) * ViewW * WindowHeigh / 2;
	ScreenPiont.Set(x, y);
	return TRUE;
}
BOOL Algorithm::WorldToScreen(FLOAT Matrix[4][4], Vec3 Object, Vec2& ScreenPiont, Vec2& ScreenText, INT WindowWindth, INT WindowHeigh) {
	FLOAT x, y1,y2, ViewW;
	ViewW = Object.x * Matrix[0][3] + Object.y * Matrix[1][3] + Object.z * Matrix[2][3] + Matrix[3][3];
	if (ViewW < 0.f) return FALSE;
	ViewW = 1.0f / ViewW;
	x = WindowWindth / 2 + (Object.x * Matrix[0][0] + Object.y * Matrix[1][0] + Object.z * Matrix[2][0] + Matrix[3][0]) * ViewW * WindowWindth / 2;
	y1 = WindowHeigh / 2 - (Object.x * Matrix[0][1] + Object.y * Matrix[1][1] + Object.z * Matrix[2][1] + Matrix[3][1]) * ViewW * WindowHeigh / 2;
	y2 = WindowHeigh / 2 - (Object.x * Matrix[0][1] + (Object.y + 1.8f) * Matrix[1][1] + Object.z * Matrix[2][1] + Matrix[3][1]) * ViewW * WindowHeigh / 2;
	ScreenPiont.Set(x, y1);
	ScreenText.Set(x, y2);
	return TRUE;
}
BOOL Algorithm::WorldToScreen(FLOAT Matrix[4][4], Vec3 Object, Vec4& ScreenRect,Vec2& ScreenPiont, INT WindowWindth, INT WindowHeigh) {
	FLOAT x, y1, y2, ViewW, h;
	ViewW = Object.x * Matrix[0][3] + Object.y * Matrix[1][3] + Object.z * Matrix[2][3] + Matrix[3][3];
	if (ViewW < 0.f) return FALSE;
	ViewW = 1.0f / ViewW;
	x = WindowWindth / 2 + (Object.x * Matrix[0][0] + Object.y * Matrix[1][0] + Object.z * Matrix[2][0] + Matrix[3][0]) * ViewW * WindowWindth / 2;
	y1 = WindowHeigh / 2 - (Object.x * Matrix[0][1] + Object.y * Matrix[1][1] + Object.z * Matrix[2][1] + Matrix[3][1]) * ViewW * WindowHeigh / 2;
	y2 = WindowHeigh / 2 - (Object.x * Matrix[0][1] + (Object.y + 2.0f) * Matrix[1][1] + Object.z * Matrix[2][1] + Matrix[3][1]) * ViewW * WindowHeigh / 2;
	h = y1 - y2;
	ScreenRect.Set(x - h / 4, y2, h / 2, h);
	ScreenPiont.Set(x, y1);
	return TRUE;
}
DOUBLE Algorithm::Torad(DOUBLE degree) {
	return degree * 3.1415926535 / 180;
}
Vec2 Algorithm::RadarCalc(Vec3 MyPos, Vec3 Pos, INT size,FLOAT AngleX)
{
	FLOAT distance;
	FLOAT angle;
	Vec2 result;
	distance = this->Get3Ddistance(Pos,MyPos);
	angle = static_cast<FLOAT>(atan2(MyPos.z - Pos.z, MyPos.x - Pos.x));
	angle = angle * 180.f / 3.1415926f;
	angle = AngleX - 90.f + angle;
	distance = distance / 200.f * 0.618f * static_cast<FLOAT>(size) * 1.55f;
	result.x = distance * static_cast<FLOAT>(sin(angle * 3.1415926f / 180));
	result.y = distance * static_cast<FLOAT>(cos(angle * 3.1415926f / 180));
	return result;
}
DOUBLE Algorithm::GetX(DOUBLE x, DOUBLE y) {
	if (x == 0 && y < 0) return (DOUBLE)-90;
	if (x == 0 && y > 0) return (DOUBLE)90;
	if (x > 0 && y == 0) return (DOUBLE)0;
	if (x < 0 && y == 0) return (DOUBLE)180;
	if (x > 0 && y < 0) {
		return (DOUBLE)(atan(x / y) * 180 / 3.1415926535 + 180);
	}
	if (x < 0 && y < 0) {
		return (DOUBLE)(-90 - atan(y / x) * 180 / 3.1415926535);
	}
	if (x > 0 && y > 0) {
		return (DOUBLE)(atan(x / y) * 180 / 3.1415926535);
	}
	if (x < 0 && y > 0) {
		return (DOUBLE)(270 - atan(y / x) * 180 / 3.1415926535);
	}
	return (DOUBLE)0;
}
DOUBLE Algorithm::GetY(DOUBLE x, DOUBLE y, DOUBLE z) {
	DOUBLE a, b = 0;
	a = z;
	b = sqrt(x * x + y * y);
	return (DOUBLE)(atan(a / b) * 180 / 3.1415926535);
}
Vec4 Algorithm::Transformation(Vec3 ev) {
	DOUBLE heading, attitude, bank, c1, s1, c2, s2, c1c2, s1s2, c3, s3;
	heading = this->Torad(ev.x);
	attitude = this->Torad(ev.y);
	bank = this->Torad(ev.z);
	c1 = cos(heading / 2);
	s1 = sin(heading / 2);
	c2 = cos(attitude / 2);
	s2 = sin(attitude / 2);
	c3 = cos(bank / 2);
	s3 = sin(bank / 2);
	c1c2 = c1 * c2;
	s1s2 = s1 * s2;
	return Vec4(static_cast<FLOAT>(c1c2 * c3 - s1s2 * s3), static_cast<FLOAT>(c1c2 * s3 + s1s2 * c3), static_cast<FLOAT>(s1 * c2 * c3 + c1 * s2 * s3), static_cast<FLOAT>(c1 * s2 * c3 - s1 * c2 * s3));
}