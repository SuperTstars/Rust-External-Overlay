#include "AimBot.h"
BOOL Aimbot::MoveTo(FLOAT speed, Vec2 target) {
	POINT pNow;
	GetCursorPos(&pNow);
	if(Draw::get().GameHwnd != WindowFromPoint(pNow)) return FALSE;
	if (speed <= 0 ||
		target.x > Draw::get().WindowWidth - 1.0f ||
		target.x < 1.0f ||
		target.y > Draw::get().WindowHeight - 1.0f ||
		target.y < 1.0f)
		return FALSE;
	Vec2 Target;
	Target.x = (target.x - Draw::get().WindowWidth / 2) / speed;
	Target.y = (target.y - Draw::get().WindowHeight / 2) / speed;
	if (Target.x < 1.0f && Target.x > 0.3f) Target.x = 1.0f;
	if (Target.x > -1.0f && Target.x < -0.3f) Target.x = -1.0f;
	if (Target.y < 1.0f && Target.y > 0.3f) Target.y = 1.0f;
	if (Target.y > -1.0f && Target.y < -0.3f) Target.y = -1.0f;
	mouse_event(1, Target.x, Target.y, 0, 0);
	return TRUE;
}
FLOAT Aimbot::CalcCollimationtoTarget(Vec2 target) {
	return Algorithm::get().Get2Ddistance(target, Vec2(Draw::get().WindowWidth / 2.f, Draw::get().WindowHeight / 2.f));
}
BOOL Aimbot::AimToTarget(Vec3 pos,FLOAT Speed) {
	if (Algorithm::get().WorldToScreen/*_AimBot*/(Matrix, pos, Point, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
		if (this->CalcCollimationtoTarget(Point) < Draw::get()._x + 35) {
			if (GetAsyncKeyState(VK_RBUTTON) < 0) MoveTo(Speed, Point);
			else return FALSE;
		}
	}
	else return FALSE;
	return TRUE;
}
BOOL Aimbot::NoRecoil(DWORD64 address) {
	if (Entity::getobject().LocalValid) {
		if (!address) return FALSE;
		DWORD64 Input = 0;
		Process::get().Read<DWORD64>(address + Offsets::PlayInput, &Input, sizeof(DWORD64));
		if (!Input)return FALSE;
		FLOAT no = 0.f;
		Process::get().Write<FLOAT>(Input + 0x64, &no, sizeof(FLOAT));
		Process::get().Write<FLOAT>(Input + 0x68, &no, sizeof(FLOAT));
	}
	return TRUE;
}
BOOL Aimbot::admin(DWORD64 address) {
	if (Entity::getobject().LocalValid) {
		if (!address) return FALSE;
		INT admin = 4;
		INT ordinary = 0;
		if (Admin) {
			Process::get().Write<INT>(address + Offsets::Flag, &admin, sizeof(INT));
		}
		else if (open == 1) {
			Process::get().Write<INT>(address + Offsets::Flag, &ordinary, sizeof(INT));
			open = 0;
		}
	}
	return TRUE;
}
BOOL Aimbot::NoFall(BOOL v) {
	if (Entity::getobject().LocalValid) {
		FLOAT a = -100.f;
		FLOAT b = -8.f;
		if (v) Process::get().Write<FLOAT>(Entity::getobject().GameAssembly + Offsets::Nofall, &a, sizeof(FLOAT));
		else Process::get().Write<FLOAT>(Entity::getobject().GameAssembly + Offsets::Nofall, &b, sizeof(FLOAT));
	}
	return TRUE;
}
BOOL Aimbot::Fly(BOOL v) {
	if (Entity::getobject().LocalValid) {
		FLOAT a = -1.f;
		FLOAT b = 0.6499999762f;
		if (v) Process::get().Write<FLOAT>(Entity::getobject().GameAssembly + Offsets::Fly, &a, sizeof(FLOAT));
		else Process::get().Write<FLOAT>(Entity::getobject().GameAssembly + Offsets::Fly, &b, sizeof(FLOAT));
	}
	return TRUE;
}
BOOL Aimbot::WallClimbing(DWORD64 address) {
	if (Entity::getobject().LocalValid) {
		if (!address) return FALSE;
		DWORD64 a = 0;
		Process::get().Read<DWORD64>(address + Offsets::BaseMovement, &a, sizeof(DWORD64));
		if (!a)return FALSE;
		FLOAT no = 0.f;
		Process::get().Write<FLOAT>(a + Offsets::GroundAngle, &no, sizeof(FLOAT));
		Process::get().Write<FLOAT>(a + Offsets::GroundAngleNew, &no, sizeof(FLOAT));
	}
	return TRUE;
}
VOID HookJmpA(DWORD64 jmp, DWORD64 tar) {
	DWORD var = tar - jmp - 5;
	BYTE* data = new BYTE[5];
	BYTE data1[] = { 233, 0, 0, 0, 0};
	memcpy(data, data1, sizeof(data1));
	memcpy(data + 1, &var, sizeof(var));
	Process::get().Write<BYTE>(jmp, data, 5);
}
VOID HookJmpB(DWORD64 jmp, DWORD64 tar) {
	DWORD var = tar - jmp - 5;
	BYTE* data = new BYTE[8];
	BYTE data1[] = { 233, 0, 0, 0, 0, 144 ,144,144, };
	memcpy(data, data1, sizeof(data1));
	memcpy(data + 1, &var, sizeof(var));
	Process::get().Write<BYTE>(jmp, data, 8);
}
VOID HookJmpC(DWORD64 jmp, DWORD64 tar) {
	DWORD var = tar - jmp - 5;
	BYTE* data = new BYTE[10];
	BYTE data1[] = { 233, 0, 0, 0, 0, 144 ,144,144,144,144};
	memcpy(data, data1, sizeof(data1));
	memcpy(data + 1, &var, sizeof(var));
	Process::get().Write<BYTE>(jmp,data,10);
}
VOID AimBotTrackA() {
	if (swag) {
		DWORD64 a = 0;
		DWORD64 Spread = Entity::getobject().GameAssembly + Offsets::HookTrack + 0x52;
		Process::get().Write<DWORD64>(Spread, &a, sizeof(DWORD64));
	}
	BYTE* data = new BYTE[77];
	BYTE data2[] = { 76, 141, 133, 128, 0, 0, 0, 15, 40, 206, 65, 80, 80, 72, 
		184, 64, 65, 48, 193, 0, 0, 0, 0, 73, 137, 0, 73, 131, 192, 4, 72, 
		184, 145, 57, 130, 193, 0, 0, 0, 0, 73, 137,  0, 73, 131, 192, 4, 
		72, 184, 16, 148, 170, 194, 0, 0, 0, 0, 73, 137, 0, 72, 184, 61, 116, 
		148, 79, 251, 127, 0, 0, 243, 15, 126, 8, 88, 65, 88 };
	memcpy(data, data2, 77);
	Process::get().Write<BYTE>(Entity::getobject().GameAssembly + Offsets::HookTrack, data, 77);
	BYTE b = 0;
	Process::get().Read<BYTE>(Entity::getobject().GameAssembly + Offsets::HookTrack, &b, sizeof(BYTE));
	if (b == 76) {
		DWORD64 c = Entity::getobject().GameAssembly + Offsets::AimCones + 0xA;
		Process::get().Write<DWORD64>(Entity::getobject().GameAssembly + Offsets::HookTrack + 0x3E, &c, sizeof(DWORD64));
		HookJmpA(Entity::getobject().GameAssembly + Offsets::HookTrack + 0x4D, Entity::getobject().GameAssembly + Offsets::AimCones + 0xA);
		HookJmpC(Entity::getobject().GameAssembly + Offsets::AimCones, Entity::getobject().GameAssembly + Offsets::HookTrack);
	}
	BYTE* data3 = new BYTE[83];
	BYTE data4[] = { 243, 15, 17, 179, 252, 0, 0, 0, 80, 65, 86, 73, 131, 198, 24, 77, 139, 54, 73, 131,
		198, 24, 77, 139, 54, 73, 131, 198, 44, 72, 184, 255, 157, 2, 0, 0, 0, 0, 0, 73, 137, 6, 73, 131,
		198, 4, 72, 184, 130, 124, 231, 160, 0, 0, 0, 0, 73, 137, 6, 73, 131, 238, 48, 73, 131, 198, 100, 
		72, 184, 61, 81, 127, 137, 0, 0, 0, 0, 73, 137, 6, 65, 94, 88 };
	memcpy(data3, data4, 83);
	Process::get().Write<BYTE>(Entity::getobject().GameAssembly + Offsets::HookTrack + 0x5A, data3, 83);
	BYTE c = 0;
	Process::get().Read<BYTE>(Entity::getobject().GameAssembly + Offsets::HookTrack + 0x5A, &c, sizeof(BYTE));
	if (c == 243) {
		HookJmpA(Entity::getobject().GameAssembly + Offsets::HookTrack + 0xAD, Entity::getobject().GameAssembly + Offsets::SendProjectileAttack + 8);
		HookJmpB(Entity::getobject().GameAssembly + Offsets::SendProjectileAttack, Entity::getobject().GameAssembly + Offsets::HookTrack + 0x5A);
	}
}
VOID AimBotTrackB() {
	BYTE data[] = { 76, 141, 133, 128, 0, 0, 0, 15, 40, 206 };
	BYTE data1[] = { 243, 15, 17, 179, 252, 0, 0, 0 };
	Process::get().Write<BYTE>(Entity::getobject().GameAssembly + Offsets::AimCones,data, sizeof(data));
	Process::get().Write<BYTE>(Entity::getobject().GameAssembly + Offsets::SendProjectileAttack, data1, sizeof(data1));
}
VOID Aimbot::OpenTrack(DWORD64 TargetAddress, Vec3 MyPos,Vec3 Pos,DWORD BoneId) {
	BYTE a = 0;
	Process::get().Read<BYTE>(Entity::getobject().GameAssembly + Offsets::AimCones, &a, sizeof(BYTE));
	if (a != 233) AimBotTrackA();
	Vec3 dis = Pos - MyPos;
	FLOAT a1 = dis.x, a2 = dis.y + 0.1f, a3 = dis.z;
	DWORD64 b = 0;
	DWORD ID = 0;
	DWORD id = BoneId;
	Process::get().Write<FLOAT>(Entity::getobject().GameAssembly + Offsets::HookTrack + 15, &a1, sizeof(FLOAT));
	Process::get().Write<FLOAT>(Entity::getobject().GameAssembly + Offsets::HookTrack + 30 + 2, &a2, sizeof(FLOAT));
	Process::get().Write<FLOAT>(Entity::getobject().GameAssembly + Offsets::HookTrack + 47 + 2, &a3, sizeof(FLOAT));
	Process::get().Read<DWORD64>(TargetAddress + 80, &b, sizeof(DWORD64));
	Process::get().Read<DWORD>(b + 16, &ID, sizeof(DWORD));
	Process::get().Write<DWORD>(Entity::getobject().GameAssembly + Offsets::HookTrack + 90 + 31, &ID, sizeof(DWORD));
	Process::get().Write<DWORD>(Entity::getobject().GameAssembly + Offsets::HookTrack + 90 + 48, &id, sizeof(DWORD));
}
VOID Aimbot::CloseTrack() {
	BYTE a = 0;
	Process::get().Read<BYTE>(Entity::getobject().GameAssembly + Offsets::AimCones, &a, sizeof(BYTE));
	if (a == 233) AimBotTrackB();
}
DWORD64 Aimbot::InitializationTrack() {
	DWORD64 TrackHookAddress = NULL,TrackJMPHook = NULL;
	TrackHookAddress = Entity::getobject().GameAssembly + Offsets::TrackHook;
	DWORD64 ReturnAddress = TrackHookAddress + 0xE;
	TrackJMPHook = Entity::getobject().GameAssembly + Offsets::TrackJMPHook;
	BYTE* data = new BYTE[48];
	BYTE data2[] = {
		128, 61, 41, 0,  0,  0,  1,  117,
		11,  15, 16, 5,  34, 0,  0,  0,
		15,  17, 67, 68, 15, 16, 123,68,
		15,  16, 48, 246,129,47, 1,  0,
		0,   2 , 255,37, 0,  0,  0,  0,
		0,   0,  0,  0,  0,  0,  0,  0
	};
	memcpy(data, data2, 40);
	memcpy(data + 40, &ReturnAddress, sizeof(ReturnAddress));
	BYTE* data3 = new BYTE[14];
	BYTE data4[] = { 255, 37, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0 };
	memcpy(data3, data4, 14);
	memcpy(data3 + 6, &TrackJMPHook, sizeof(TrackJMPHook));
	Process::get().Write<BYTE>(TrackJMPHook, data, 48);
	Process::get().Write<BYTE>(TrackHookAddress, data3, 14);
	return TrackJMPHook + 0x30;
}
Vec2 Aimbot::GetRecoil(DWORD64 address) {
	DWORD64 PlayerInput = 0;
	Vec2 mousepos{};
	Process::get().Read<DWORD64>(address + Offsets::PlayInput, &PlayerInput, sizeof(DWORD64));
	Process::get().Read<Vec2>(PlayerInput + Offsets::RecoilAngles, &mousepos, sizeof(Vec2));
	return mousepos;
}
BOOL Aimbot::StarTrack(Vec3 pos, DWORD64 LocalPlayer, DWORD64 address) {
	if (!LocalPlayer) return FALSE;
	if (!address) return FALSE;
	DOUBLE aglex, agley = 0;
	Vec3 test{}, mypos{};
	Vec2 Recoil{};
	Vec4 QuatAngles{};
	DWORD64 a, b, c, d, e = 0;
	Process::get().Read<DWORD64>(LocalPlayer + Offsets::pObjectPos->a, &a, sizeof(DWORD64));
	Process::get().Read<DWORD64>(a + Offsets::pObjectPos->b, &b, sizeof(DWORD64));
	Process::get().Read<DWORD64>(b + Offsets::pObjectPos->c, &c, sizeof(DWORD64));
	Process::get().Read<DWORD64>(c + Offsets::pObjectPos->d, &d, sizeof(DWORD64));
	Process::get().Read<DWORD64>(d + Offsets::pObjectPos->e, &e, sizeof(DWORD64));
	Process::get().Read<Vec3>(e + Offsets::pObjectPos->f, &mypos, sizeof(Vec3));
	aglex = Algorithm::get().GetX((pos - mypos).x, (pos - mypos).z);
	agley = -Algorithm::get().GetY((pos - mypos).x, (pos - mypos).z, (pos - mypos).y);
	test.x = aglex;
	test.y = agley;
	test.z = 0.f;
	QuatAngles = Algorithm::get().Transformation(test);
	FLOAT aa = QuatAngles.x, bb = -QuatAngles.y, cc = QuatAngles.w, dd = QuatAngles.h;
	if (aa > 1.f || aa < -1.f || bb > 1.f || bb < -1.f || cc > 1.f || cc < -1.f || dd > 1.f || dd < -1.f) return FALSE;
	Process::get().Write<FLOAT>(address, &dd, sizeof(FLOAT));
	Process::get().Write<FLOAT>(address + 4, &cc, sizeof(FLOAT));
	Process::get().Write<FLOAT>(address + 8, &bb, sizeof(FLOAT));
	Process::get().Write<FLOAT>(address + 12, &aa, sizeof(FLOAT));
	return TRUE;
}
VOID Aimbot::InternalAimBot(DWORD64 LocalPlayer,Vec3 pos) {
	if (!LocalPlayer) return ;
	Vec3 mypos{};
	Vec2 agle{};
	DWORD64 a, b, c, d, e ,f = 0;
	Process::get().Read<DWORD64>(LocalPlayer + Offsets::PlayInput, &f, sizeof(DWORD64));
	Process::get().Read<DWORD64>(LocalPlayer + Offsets::pObjectPos->a, &a, sizeof(DWORD64));
	Process::get().Read<DWORD64>(a + Offsets::pObjectPos->b, &b, sizeof(DWORD64));
	Process::get().Read<DWORD64>(b + Offsets::pObjectPos->c, &c, sizeof(DWORD64));
	Process::get().Read<DWORD64>(c + Offsets::pObjectPos->d, &d, sizeof(DWORD64));
	Process::get().Read<DWORD64>(d + Offsets::pObjectPos->e, &e, sizeof(DWORD64));
	Process::get().Read<Vec3>(e + Offsets::pObjectPos->f, &mypos, sizeof(Vec3));
	agle.Set(-Algorithm::get().GetY((pos - mypos).x, (pos - mypos).z, (pos - mypos).y),Algorithm::get().GetX((pos - mypos).x, (pos - mypos).z));
	if (Algorithm::get().WorldToScreen_AimBot(Matrix, pos, Point, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
		if (this->CalcCollimationtoTarget(Point) < 200.f) Process::get().Write<Vec2>(f + Offsets::Angles, &agle, sizeof(Vec2));
	}
}
DWORD64 Aimbot::GetWeapon(DWORD64 address,BOOL v) {
	if (!address) return 0;
	DWORD64 a = 0, b = 0, f = 0, Inventory = 0, Belt = 0, ItemList = 0, Items = 0, weapon = 0;
	INT WeaponUID, UID = 0;
	INT Count = 0;
	Process::get().Read<INT>(address + Offsets::pActiveWeaponOffsets->a, &WeaponUID, sizeof(INT));
	Process::get().Read<DWORD64>(address + Offsets::pActiveWeaponOffsets->b, &Inventory, sizeof(DWORD64));
	Process::get().Read<DWORD64>(Inventory + Offsets::pActiveWeaponOffsets->c, &Belt, sizeof(DWORD64));
	Process::get().Read<DWORD64>(Belt + Offsets::pActiveWeaponOffsets->d, &ItemList, sizeof(DWORD64));
	Process::get().Read<DWORD64>(ItemList + 0x10, &Items, sizeof(DWORD64));
	Process::get().Read<INT>(ItemList + 0x18, &Count, sizeof(INT));
	for (int i = 0; i < Count; i++)
	{
		Process::get().Read<DWORD64>(Items + Offsets::pActiveWeaponOffsets->f + (i * 0x8), &weapon, sizeof(DWORD64));
		Process::get().Read<INT>(weapon + Offsets::pActiveWeaponOffsets->e, &UID, sizeof(INT));
		if (UID == WeaponUID)
		{
			if (weapon) {
				if(!v) return weapon;
				else {
					Process::get().Read<DWORD64>(weapon + 0x20, &a, sizeof(DWORD64));
					Process::get().Read<DWORD64>(a + 0x18, &b, sizeof(DWORD64));
					return b;
				}

			}
			else return 0;
		}
	}
	return 0;
}
FLOAT Aimbot::GetWeaponBulletSpeed(DWORD64 address) {
	if (!address) return 0.f;
	DWORD64 a = 0, b = 0, f = 0, Inventory = 0, Belt = 0, ItemList = 0, Items = 0, weapon = 0, ItemDefinition = 0, TranslatePhrase = 0;
	INT WeaponUID, UID = 0;
	NameList str;
	INT Count = 0;
	FLOAT c = 0.f;
	Process::get().Read<INT>(address + Offsets::pActiveWeaponOffsets->a, &WeaponUID, sizeof(INT));
	Process::get().Read<DWORD64>(address + Offsets::pActiveWeaponOffsets->b, &Inventory, sizeof(DWORD64));
	Process::get().Read<DWORD64>(Inventory + Offsets::pActiveWeaponOffsets->c, &Belt, sizeof(DWORD64));
	Process::get().Read<DWORD64>(Belt + Offsets::pActiveWeaponOffsets->d, &ItemList, sizeof(DWORD64));
	Process::get().Read<DWORD64>(ItemList + 0x10, &Items, sizeof(DWORD64));
	Process::get().Read<INT>(ItemList + 0x18, &Count, sizeof(INT));
	for (int i = 0; i < Count; i++)
	{
		Process::get().Read<DWORD64>(Items + Offsets::pActiveWeaponOffsets->f + (i * 0x8), &weapon, sizeof(DWORD64));
		Process::get().Read<INT>(weapon + Offsets::pActiveWeaponOffsets->e, &UID, sizeof(INT));
		if (UID == WeaponUID)
		{
			if (weapon) {
				Process::get().Read<DWORD64>(weapon + 0x20, &a, sizeof(DWORD64));
				Process::get().Read<DWORD64>(a + 0x18, &b, sizeof(DWORD64));
			}
			else return 0.f;
		}
	}
	switch (b)
	{
	case 1545779598://ak
		c = 375.f;
		break;
	case 2482412119:
		c = 375.f;
		break;
	case 3390104151:
		c = 375.f;
		break;
	case 28201841:
		c = 375.f * 1.25f;
		break;
	case 2225388408:
		c = 375.f * 1.4f;
		break;
	case 1588298435://bolt
		c = 375.f * 1.8f;
		break;
	case 3516600001://L96
		c = 375.f * 3.2f;
		break;
	case 1318558775:
		c = 300.f * 0.8f;
		break;
	case 1953903201://¶¤Ç¹
		c = 50.f;
		break;
	case 3529783679:
		c = 100.f;
		break;
	case 2927685355:
		c = 100.f;
		break;
	case 192490795:
		c = 100.f;
		break;
	case 3171493472:
		c = 100.f;
		break;
	case 795371088:
		c = 100.f;
		break;
	case 442886268:
		c = 100.f;
		break;
	case 226994994:
		c = 100.f;
		break;
	case 1796682209:
		c = 300.f * 0.8f;
		break;
	case 2536594571:
		c = 300.f;
		break;
	case 3442404277:
		c = 300.f;
		break;
	case 818877484://p250
		c = 300.f;
		break;
	case 1373971859:
		c = 300.f;
		break;
	case 649912614:
		c = 300.f;
		break;
	case 1443579727:
		c = /*66.f*/63.f;
		break;
	case 1965232394:
		c = 100.f;
		break;
	case 1801741824:
		c = 656.25f;
		break;
	case 1798229440:
		c = 1125.f;
		break;
	case 1802481984:
		c = 469.f;
		break;
	case 1798228096:
		c = 300.f;
		break;
	case 1802077840:
		c = 100.f;
		break;
	case 1798227968:
		c = 100.f;
		break;
	default:
		c = 0.f;
		break;
	}

	return c;
}
BOOL Aimbot::GeMetee(DWORD64 address) {
	if (Entity::getobject().LocalValid) {
		if (!address) return FALSE;
		DWORD64 a = 0;
		FLOAT b = 0;
		FLOAT c = 3.0f, d = 1.0f;
		BOOL q = FALSE;
		BOOL w = TRUE;
		DWORD64 weapon = this->GetWeapon(address, TRUE);
		if (weapon == 3042908079 ||
			weapon == 2992837901 ||
			weapon == 1814288539 ||
			weapon == 1488979457 ||
			weapon == 1104520648 ||
			weapon == 1711033574 ||
			weapon == 4032376893 ||
			weapon == 2514164731 ||
			weapon == 2788569439 ||
			weapon == 171931394 ||
			weapon == 2710999350 ||
			weapon == 1540934679 ||
			weapon == 1491189398 ||
			weapon == 2328218800 ||
			weapon == 1602646136 ||
			weapon == 2825389095 ||
			weapon == 2040726127 ||
			weapon == 3157102211 ||
			weapon == 4100458014 ||
			weapon == 1090916276 ||
			weapon == 2315967767 ||
			weapon == 1326180354) {
			this->weaponptr = this->GetWeapon(address, FALSE);
			if (this->isweaponptr != this->weaponptr) {
				Process::get().Read<DWORD64>(this->weaponptr + Offsets::HeldEntity, &a, sizeof(DWORD64));
				Process::get().Write<FLOAT>(a + Offsets::maxDistance, &c, sizeof(FLOAT));
				Process::get().Write<FLOAT>(a + Offsets::AttackRadius, &d, sizeof(FLOAT));
				Process::get().Write<BOOL>(a + Offsets::BlockSprintOnAttack, &q, sizeof(BOOL));
				this->isweaponptr = this->weaponptr;
			}
		}
	}
	return FALSE;
}
VOID Aimbot::NoSway(DWORD64 address,BOOL v) {
	if (!address) return;
	FLOAT a = 0.99f, b = 0.f;
	if (v) {
		Process::get().Write<FLOAT>(address + Offsets::clothingAccuracyBonus, &a, sizeof(FLOAT));
		OpenNoSway = TRUE;
	}
	else if (OpenNoSway == TRUE) {
		Process::get().Write<FLOAT>(address + Offsets::clothingAccuracyBonus, &b, sizeof(FLOAT));
		OpenNoSway = FALSE;
	}
}
Vec3 Aimbot::PredictionAim(DWORD64 LocalPlayerAddress,Vec3 MyPos,Vec3 Pos,Vec3 Speed ,Vec2 Mouse) {
	Vec3 pos{ };
	FLOAT dis = Algorithm::get().Get3Ddistance(MyPos, Pos);
	FLOAT WeaponBulletSpeed = this->GetWeaponBulletSpeed(LocalPlayerAddress);
	pos = Pos;
	if (WeaponBulletSpeed <= 0) return pos;
	FLOAT time = dis / WeaponBulletSpeed;
	Vec3 PredictPos = Speed * 0.995f * time;
	pos = pos + PredictPos;
	FLOAT Angle = Mouse.x;
	float cof = 1;
	if (Angle >= 10 && dis <= 100) cof = static_cast<FLOAT>(cos(DEG2RAD(Angle))) * 0.1f;
	else if (Angle >= 0) cof = static_cast<FLOAT>(cos(DEG2RAD(Angle))) * 0.9f;
	pos.y += (6.4f * time * time) * cof;
	return pos;
}
DWORD64 Aimbot::getComponent(DWORD64 address,LPSTR b1, LPSTR c1) {
	DWORD64 a, b, c,d,e,f,g;
	char name[128] = { 0 };
	if (!address) return 0;
	Process::get().Read<DWORD64>(address + 0x30, &a, sizeof(DWORD64));
	for (int i = 0; i < 20; i++) {
		Process::get().Read<DWORD64>(a + 16 * i + 24, &b, sizeof(DWORD64));
		Process::get().Read<DWORD64>(b + 0x28, &c, sizeof(DWORD64));
		if (!c) break;
		Process::get().Read<DWORD64>(c, &d, sizeof(DWORD64));
		Process::get().Read<DWORD64>(d + 0x10, &e, sizeof(DWORD64));
		Process::get().Read<CHAR>(e, name, sizeof(name));
		if (strstr(name, b1) || strstr(name, c1)) {
			return c;
		}
	}
	return 0;
}
BulletInfo Aimbot::GetBulletInfo(DWORD64 address) {
	DWORD64 a, ItemDefinition, componentList, component, projectile;
	FLOAT f, modifier;
	BulletInfo h{ };
	DWORD64 j = this->GetWeapon(address, FALSE);
	DWORD64 k = this->GetWeapon(address, TRUE);
	Process::get().Read<DWORD64>(j + Offsets::HeldEntity, &a, sizeof(DWORD64));
	Process::get().Read<FLOAT>(a + Offsets::modifier, &modifier, sizeof(FLOAT));
	if (k == 1443579727 ||k == 1965232394) {
		modifier = modifier + 0.5f;
		h.IsCowBow = TRUE;
	}
	DWORD64 b, c, d, e,g;
	DWORD64 b1, c1, d1;
	Process::get().Read<DWORD64>(j + Offsets::HeldEntity, &g, sizeof(DWORD64));
	Process::get().Read<DWORD64>(g + Offsets::primaryMagazine, &b, sizeof(DWORD64));
	Process::get().Read<DWORD64>(b + 0x20, &c, sizeof(DWORD64));
	Process::get().Read<DWORD64>(c + 0x10, &d, sizeof(DWORD64));
	Process::get().Read<DWORD64>(d + 0x30, &e, sizeof(DWORD64));
	component = getComponent(e, "ItemModProjectile", "ItemModProjectileSpawn");
	Process::get().Read<DWORD64>(component + 0x18, &b1, sizeof(DWORD64));
	Process::get().Read<DWORD64>(b1 + 0x18, &c1, sizeof(DWORD64));
	Process::get().Read<DWORD64>(c1 + 0x10, &d1, sizeof(DWORD64));
	projectile = getComponent(d1,"Projectile","xxxxxx");
	if (projectile) {
		FLOAT q, w, t, r;
		Process::get().Read<FLOAT>(projectile + 36, &q, sizeof(FLOAT));
		h.Drag = q;
		Process::get().Read<FLOAT>(component + 52, &w, sizeof(FLOAT));
		Process::get().Read<FLOAT>(component + 56, &t, sizeof(FLOAT));
		h.BulletSpeed = (w + t) * modifier;
		Process::get().Read<FLOAT>(projectile + 40, &r, sizeof(FLOAT));
		h.GravityModifier = r;
		return h;
	}
	return h;
}
DOUBLE Aimbot::CalcBulletDrop(DOUBLE height,DOUBLE DepthPlayerTarget,FLOAT vel,FLOAT gra) {
	DOUBLE pitch, BulletVelocityXY, Time, TotalVerticalDrop;
	pitch = atan2(height, DepthPlayerTarget);
	BulletVelocityXY = vel * cos(pitch);
	Time = DepthPlayerTarget / BulletVelocityXY;
	TotalVerticalDrop = 0.5f * gra * Time * Time;
	return TotalVerticalDrop * 10;
}
Vec3 Aimbot::PredictionAimEx(Vec3 MyPos ,Vec3 Target,Vec3 tar, BulletInfo info) {
	FLOAT dis = Algorithm::get().Get3Ddistance(MyPos, Target);
	FLOAT BulletTime = dis / info.BulletSpeed;
	Vec3 vel{}, PredictVel{}, dir{}, Pos{};
	vel.Set(tar.x, 0, tar.z);
	PredictVel = vel * BulletTime;
	Target = Target + PredictVel;
	DOUBLE height = Target.y - MyPos.y;
	dir = Target - MyPos;
	FLOAT DepthPlayerTarget = sqrt(dir.x * dir.x + dir.y * dir.y);
	FLOAT drop = this->CalcBulletDrop(height, DepthPlayerTarget, info.BulletSpeed, info.GravityModifier);
	if (info.IsCowBow) drop = (info.Drag + info.GravityModifier) * 9.81f * BulletTime * BulletTime;
	Target.y = Target.y + drop;
	Pos = Target;
	return Pos;
}