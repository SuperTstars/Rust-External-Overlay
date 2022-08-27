#pragma once
#include "Includes.h"
#include "Renderer.h"
#include "MathFunc.h"
#include "RustData.h"
#include "Struct.h"
#include "global.h"
class Aimbot : public Singleton<Aimbot> {
public:
	BOOL MoveTo(FLOAT speed, Vec2 target);
	BOOL AimToTarget(Vec3 pos, FLOAT Speed);
	Vec3 PredictionAim(DWORD64 LocalPlayerAddress, Vec3 MyPos, Vec3 Pos, Vec3 Speed, Vec2 Mouse);
	FLOAT GetWeaponBulletSpeed(DWORD64 LocalPlayerAddress);
	FLOAT CalcCollimationtoTarget(Vec2 target);
	BOOL NoRecoil(DWORD64 address);
	DWORD64 GetWeapon(DWORD64 address, BOOL v);
	BOOL WallClimbing(DWORD64 address);
	BOOL admin(DWORD64 address);
	BOOL NoFall(BOOL open);
	DWORD64 InitializationTrack();
	BOOL StarTrack(Vec3 pos, DWORD64 LocalPlayer, DWORD64 address);
	Vec2 GetRecoil(DWORD64 address);
	VOID InternalAimBot(DWORD64 address, Vec3 pos);
	BOOL GeMetee(DWORD64 address);
	VOID NoSway(DWORD64 address, BOOL v);
	BOOL Fly( BOOL v);
	VOID OpenTrack(DWORD64 TargetAddress, Vec3 MyPos, Vec3 Pos,DWORD BoneId);
	VOID CloseTrack();
	BulletInfo GetBulletInfo(DWORD64 address);
	Vec3 PredictionAimEx(Vec3 MyPos, Vec3 Target, Vec3 tar, BulletInfo info);
	DWORD64 getComponent(DWORD64 address, LPSTR b1, LPSTR c1);
	DOUBLE CalcBulletDrop(DOUBLE height, DOUBLE DepthPlayerTarget, FLOAT vel, FLOAT gra);
	BOOL open = FALSE;
	BOOL OpenNoSway = FALSE;
	DWORD64 weaponptr = NULL;
	DWORD64 isweaponptr = NULL;
};