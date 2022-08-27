#include "RustData.h"
#include "Renderer.h"
#include "Process.h"
#include "MathFunc.h"
#include "global.h"
static std::vector<ObjectInfo> AllObjectList{ 0 };
static bool localExit = false;
static bool track = false;
static bool TrackLock = false;
Vec3 Object::GetPosition(DWORD64 address, INT Bone) {
	DWORD64 PalyerModel, BoneTransform, BoneValue, addr = 0;
	if (!address) return Vec3{ 0.f, 0.f, 0.f };
	Process::get().Read<DWORD64>(address + 0x128, &PalyerModel, sizeof(DWORD64));
	Process::get().Read<DWORD64>(PalyerModel + 0x48, &BoneTransform, sizeof(DWORD64));
	Process::get().Read<DWORD64>(BoneTransform + (0x20 + (Bone * 0x8)), &BoneValue, sizeof(DWORD64));
	Process::get().Read<DWORD64>(BoneValue + 0x10, &addr, sizeof(DWORD64));
	if(!addr) return Vec3{ 0.f, 0.f, 0.f };
	struct Matrix34 { BYTE vec0[16]; BYTE vec1[16]; BYTE vec2[16]; };
	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };
	INT Index;
	Process::get().Read<INT>(addr + 0x40, &Index, sizeof(INT));
	DWORD64 pTransformData;
	Process::get().Read<DWORD64>(addr + 0x38, &pTransformData, sizeof(DWORD64));
	if (!pTransformData) return Vec3{ 0.f, 0.f, 0.f };
	DWORD64 transformData[2] = { 0 };
	Process::get().Read<DWORD64>(pTransformData + 0x18, &transformData[0], sizeof(DWORD64));
	Process::get().Read<DWORD64>(pTransformData + 0x20, &transformData[1], sizeof(DWORD64));
	UINT sizeMatriciesBuf = 48 * Index + 48;
	UINT sizeIndicesBuf = 4 * Index + 4;
	PVOID pMatriciesBuf = malloc(sizeMatriciesBuf);
	PVOID pIndicesBuf = malloc(sizeIndicesBuf);
	if (pMatriciesBuf && pIndicesBuf) {
		Process::get().Read<VOID>(transformData[0], pMatriciesBuf, sizeMatriciesBuf);
		Process::get().Read<VOID>(transformData[1], pIndicesBuf, sizeIndicesBuf);
		__m128 result = *(__m128*)((ULONG64)pMatriciesBuf + 0x30 * Index);
		INT transformIndex = *(INT*)((ULONG64)pIndicesBuf + 0x4 * Index);
		while (transformIndex >= 0) {
			Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);
			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);
			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix34.vec0));
			try {
				transformIndex = *(INT*)((ULONG64)pIndicesBuf + 0x4 * transformIndex);
			}
			catch (...) {

			}
		}
		free(pMatriciesBuf);
		free(pIndicesBuf);
		return Vec3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}
	return Vec3(0,0,0);
}
BOOL Object::AimBotThread() {
	Mtx.lock();
	if (LocalValid) {
		INT WeaponUID = 0;
		Process::get().Read<INT>(LocalPlayerAddress + Offsets::pActiveWeaponOffsets->a, &WeaponUID, sizeof(INT));
		if (WeaponUID) Ishold = TRUE; else Ishold = FALSE;
		if (AimBot && TargetAddress) {
			DWORD64 a, b, c, d, e, f ,g = 0;
			DWORD64 ma, mb, held = 0;
			Vec3 newpos{ };
			Vec3 Speed, pos{ };
			Vec2 Mouse;
			DWORD64 Input = 0;
			INT h = 0,mh = 0;
			BOOL IsOpenMenu = 1;
			FLOAT down = 0;
			Process::get().Read<DWORD64>(TargetAddress + Offsets::PlayerModel, &f, sizeof(DWORD64));
			Process::get().Read<DWORD64>(LocalPlayerAddress + Offsets::PlayerModel, &ma, sizeof(DWORD64));
			Process::get().Read<BOOL>(TargetAddress + Offsets::IsDead, &Deaded, sizeof(BOOL));
			Process::get().Read<FLOAT>(TargetAddress + Offsets::ObjectHealth, &TargetHealth, sizeof(FLOAT));
			Process::get().Read<CHAR>(TargetAddress + Offsets::Flag, &Flag, sizeof(CHAR));
			Process::get().Read<DWORD64>(LocalPlayerAddress + Offsets::PlayInput, &Input, sizeof(DWORD64));
			Process::get().Read<BOOL>(Input + Offsets::IsOpenMenu, &IsOpenMenu, sizeof(BOOL));
			Process::get().Read<DWORD64>(ma + Offsets::ModelState, &mb, sizeof(DWORD64));
			Process::get().Read<INT>(mb + Offsets::ModelFlag, &mh, sizeof(INT));
			if (mh == 5 || mh == 133) down = 0.45f; else down = -0.1f;
			if (IsOpenMenu) {
				if (Deaded == FALSE && Flag != 64 && Flag != 16) {
					Process::get().Read<Vec3>(f + Offsets::Velocity, &Speed, sizeof(Vec3));
					Process::get().Read<Vec2>(Input + Offsets::Angles, &Mouse, sizeof(Vec2));
					Process::get().Read<DWORD64>(TargetAddress + Offsets::pObjectPos->a, &a, sizeof(DWORD64));
					Process::get().Read<DWORD64>(a + Offsets::pObjectPos->b, &b, sizeof(DWORD64));
					Process::get().Read<DWORD64>(b + Offsets::pObjectPos->c, &c, sizeof(DWORD64));
					Process::get().Read<DWORD64>(c + Offsets::pObjectPos->d, &d, sizeof(DWORD64));
					Process::get().Read<DWORD64>(d + Offsets::pObjectPos->e, &e, sizeof(DWORD64));
					Process::get().Read<Vec3>(e + Offsets::pObjectPos->f, &pos, sizeof(Vec3));
					Process::get().Read<DWORD64>(f + Offsets::ModelState , &g, sizeof(DWORD64));
					Process::get().Read<INT>(g + Offsets::ModelFlag, &h, sizeof(INT));
					v = Aimbot::get().GetWeapon(LocalPlayerAddress, TRUE);
					if (v == 1443579727 || v == 1965232394) Cow = TRUE; else Cow = FALSE;
					if (Algorithm::get().Get3Ddistance(MyPos, pos) > 0.f && Algorithm::get().Get3Ddistance(MyPos, pos) < 60.f) {
						if (!Cow) {
							if (AimBotType) {
								if (abs(Speed.x) > 3.f) pos.x = pos.x + Speed.x * 0.04f;
								if (abs(Speed.z) > 3.f) pos.z = pos.z + Speed.z * 0.04f;
								if (Algorithm::get().Get3Ddistance(MyPos, pos) < 25.f) {
									if (abs(Speed.x) > 3.f) pos.x = pos.x + Speed.x * 0.05f;
									if (abs(Speed.z) > 3.f) pos.z = pos.z + Speed.z * 0.05f;
								}
							}
							else {
								if (abs(Speed.x) > 3.f) pos.x = pos.x + Speed.x * 0.015f;
								if (abs(Speed.z) > 3.f) pos.z = pos.z + Speed.z * 0.015f;
								if (Algorithm::get().Get3Ddistance(MyPos, pos) < 25.f) {
									if (abs(Speed.x) > 3.f) pos.x = pos.x + Speed.x * 0.025f;
									if (abs(Speed.z) > 3.f) pos.z = pos.z + Speed.z * 0.025f;
								}
							}
						}
						else if(v == 1443579727) {
							if (Algorithm::get().Get3Ddistance(MyPos, pos) >= 40.f) {
								if (abs(Speed.x) > 1.f) pos.x = pos.x + Speed.x * 0.05f;
								if (abs(Speed.z) > 1.f) pos.z = pos.z + Speed.z * 0.05f;
								if (abs(Speed.x) > 3.f) pos.x = pos.x + Speed.x * 0.26f;
								if (abs(Speed.z) > 3.f) pos.z = pos.z + Speed.z * 0.26f;
							}
							if (Algorithm::get().Get3Ddistance(MyPos, pos) >= 20.f && Algorithm::get().Get3Ddistance(MyPos, pos) < 40.f) {
								if (abs(Speed.x) > 1.f)pos.x = pos.x + Speed.x * 0.23f;
								if (abs(Speed.z) > 1.f)pos.z = pos.z + Speed.z * 0.23f;
							}
							if (Algorithm::get().Get3Ddistance(MyPos, pos) >= 10.f && Algorithm::get().Get3Ddistance(MyPos, pos) < 20.f) {
								if (abs(Speed.x) > 1.f)pos.x = pos.x + Speed.x * 0.12f;
								if (abs(Speed.z) > 1.f)pos.z = pos.z + Speed.z * 0.12f;
							}
							if (Algorithm::get().Get3Ddistance(MyPos, pos) < 10.f) {
								if (abs(Speed.x) > 1.f)pos.x = pos.x + Speed.x * 0.08f;
								if (abs(Speed.z) > 1.f)pos.z = pos.z + Speed.z * 0.08f;
							}
							if (abs(Speed.x) > 0.f || abs(Speed.z) > 0.f) pos.y = pos.y - 0.15f;
						}
						else if (v == 1965232394) {
							if (Algorithm::get().Get3Ddistance(MyPos, pos) >= 40.f) {
								if (abs(Speed.x) > 1.f) pos.x = pos.x + Speed.x * 0.05f;
								if (abs(Speed.z) > 1.f) pos.z = pos.z + Speed.z * 0.05f;
								if (abs(Speed.x) > 3.f) pos.x = pos.x + Speed.x * 0.18f;
								if (abs(Speed.z) > 3.f) pos.z = pos.z + Speed.z * 0.18f;
							}
							if (Algorithm::get().Get3Ddistance(MyPos, pos) >= 20.f && Algorithm::get().Get3Ddistance(MyPos, pos) < 40.f) {
								if (abs(Speed.x) > 1.f)pos.x = pos.x + Speed.x * 0.12f;
								if (abs(Speed.z) > 1.f)pos.z = pos.z + Speed.z * 0.12f;
							}
							if (Algorithm::get().Get3Ddistance(MyPos, pos) >= 10.f && Algorithm::get().Get3Ddistance(MyPos, pos) < 20.f) {
								if (abs(Speed.x) > 1.f)pos.x = pos.x + Speed.x * 0.08f;
								if (abs(Speed.z) > 1.f)pos.z = pos.z + Speed.z * 0.08f;
							}
							if (Algorithm::get().Get3Ddistance(MyPos, pos) < 10.f) {
								if (abs(Speed.x) > 1.f)pos.x = pos.x + Speed.x * 0.02f;
								if (abs(Speed.z) > 1.f)pos.z = pos.z + Speed.z * 0.02f;
							}
							if (abs(Speed.x) > 0.f || abs(Speed.z) > 0.f) pos.y = pos.y - 0.2f;
						}
					}
					if (v == 1443579727) pos.y = pos.y + 0.15f;
					if (v == 1965232394) pos.y = pos.y - 0.1f;
					if (h == 5 || h == 133) Down = 0.4f; else Down = 0.f;
					if (GetAsyncKeyState(VK_RBUTTON) < 0) IsMove = TRUE; else IsMove = FALSE;
					if (Ishold) {
						//newpos = Aimbot::get().PredictionAim(LocalPlayerAddress, MyPos, pos, Speed, Mouse);
						newpos = Aimbot::get().PredictionAimEx(MyPos, pos, Speed, Aimbot::get().GetBulletInfo(LocalPlayerAddress));
						if (AimBotType) {
							if (IsMove) {
								newpos.y = newpos.y + 1.6f;
								if (Algorithm::get().Get3Ddistance(MyPos, pos) <= 80.f) newpos.y = newpos.y - 0.1f;
								if (Position) newpos.Set(newpos.x, newpos.y - Down, newpos.z); else newpos.Set(newpos.x, newpos.y - Down - 0.3f, newpos.z);
								Aimbot::get().AimToTarget(newpos, 5.f);
							}
						}
						else {
							if (IsMove) {
								DWORD id = 0;
								if (Position) id = 698017942; else id = 2811218643;
								if (Algorithm::get().Get3Ddistance(MyPos, pos) >= 140.f) newpos.y = newpos.y + 0.15f;
								if (!Position) newpos.Set(newpos.x, newpos.y + down - Down - 0.3f, newpos.z); else newpos.Set(newpos.x, newpos.y + down - Down, newpos.z);
								Aimbot::get().OpenTrack(TargetAddress, MyPos, newpos, id);
							}
							else Aimbot::get().CloseTrack();
						}
					}
					else IsMove = FALSE;
				}
				if (Deaded) IsMove = FALSE;
				/*if (Deaded == FALSE && Flag != 64 && Flag != 16) {
					Process::get().Read<Vec3>(f + Offsets::Velocity, &Speed, sizeof(Vec3));
					Process::get().Read<Vec2>(Input + Offsets::Angles, &Mouse, sizeof(Vec2));
					if (GetAsyncKeyState(VK_RBUTTON) < 0) IsMove = TRUE; else IsMove = FALSE;
					if (Ishold) {
						if (IsMove) {
							newpos = Aimbot::get().PredictionAim(LocalPlayerAddress, MyPos, this->GetPosition(TargetAddress, BonePosition), Speed, Mouse);
							Aimbot::get().AimToTarget(TargetAddress, newpos, 5.f);
						}
					}
				}*/
			}
			else IsMove = FALSE;
		}
		else {
			IsMove = FALSE; 
		}
		if (Wall) Aimbot::get().WallClimbing(LocalPlayerAddress);
	}
	else {
		IsMove = FALSE;
	}
	Mtx.unlock();
	return TRUE;
}
BOOL Object::GetBase() {
	DWORD64 a, b, c = 0;
	Process::get().Read<DWORD64>(this->GameAssembly + Offsets::BaseNetworkableOffset, &this->BaseNetworkable, sizeof(DWORD64));
	Process::get().Read<DWORD64>(this->BaseNetworkable + Offsets::pBaseOffsets->a, &a, sizeof(DWORD64));
	Process::get().Read<DWORD64>(a + Offsets::pBaseOffsets->b, &b, sizeof(DWORD64));
	Process::get().Read<DWORD64>(b + Offsets::pBaseOffsets->c, &c, sizeof(DWORD64));
	Process::get().Read<DWORD64>(c + Offsets::pBaseOffsets->d, &this->base, sizeof(DWORD64));
	if (!this->base) return FALSE;
	return TRUE;
}
BOOL Object::GetObjectCount() {
	this->get().ObjectCount = 0;
	Process::get().Read<INT>(this->base + Offsets::ObjectCount, &this->get().ObjectCount, sizeof(INT));
	if (!this->get().ObjectCount) return FALSE;
	return TRUE;
}
VOID Object::GetObjectEntityList() {
	DWORD64 ListBuffer = 0;
	DWORD64 address = 0;
	localExit = false;
	std::vector<ObjectInfo> ObjectArray = { };
	Process::get().Read<DWORD64>(this->base + Offsets::ObjectList, &ListBuffer, sizeof(DWORD64));
	for (auto i = 0; i < this->get().ObjectCount; i++) {
		DWORD64 a, b, c, d, e = 0;
		static char ClassName[128] = { '\0' };
		Process::get().Read<DWORD64>(ListBuffer + 0x20 + (i * 8), &address, sizeof(DWORD64));
		if (address <= 100000) continue;
		Process::get().Read<DWORD64>(address + Offsets::pEntityListOffsets->a, &a, sizeof(DWORD64));
		if (a <= 100000) continue;
		Process::get().Read<DWORD64>(a + Offsets::pEntityListOffsets->b, &b, sizeof(DWORD64));
		if (b <= 100000) continue;
		Process::get().Read<DWORD64>(b + Offsets::pEntityListOffsets->c, &c, sizeof(DWORD64));
		Process::get().Read<CHAR>(c, ClassName, 128);
		ObjectInfo player;
		if (strstr(ClassName, "LocalPlayer")) {
			DWORD64 a1, b1, c1, d1, e1,f1 = 0;
			localExit = true;
			player.ObjectAddress = address;
			player.ObjectType = LocalPlayer;
			Process::get().Read<DWORD64>(address + Offsets::pObjectPos->a, &a1, sizeof(DWORD64));
			Process::get().Read<DWORD64>(a1 + Offsets::pObjectPos->b, &b1, sizeof(DWORD64));
			Process::get().Read<DWORD64>(b1 + Offsets::pObjectPos->c, &c1, sizeof(DWORD64));
			Process::get().Read<DWORD64>(c1 + Offsets::pObjectPos->d, &d1, sizeof(DWORD64));
			Process::get().Read<DWORD64>(d1 + Offsets::pObjectPos->e, &e1, sizeof(DWORD64));
			Process::get().Read<Vec3>(e1 + Offsets::pObjectPos->f, &player.LocalPlayer.pos, sizeof(Vec3));
			Process::get().Read<DWORD64>(address + Offsets::MyTeam, &MyTeam, sizeof(DWORD64));
			Process::get().Read<DWORD64>(MyTeam + Offsets::MyNowTeam, &NowMyTeam, sizeof(DWORD64));
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "player.prefab") && !strstr(ClassName, "prop") && !strstr(ClassName, "corpse")) {
			player.ObjectAddress = address;
			player.ObjectType = Player;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "player_corpse.prefab") || strstr(ClassName, "item_drop_backpack.prefab") || strstr(ClassName, "scientist_corpse.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = PlayerCorpse;
			ObjectArray.push_back(player);
		}
		else if (
			strstr(ClassName, "scientistnpc_cargo_turret_lr300") ||
			strstr(ClassName, "zombie") ||
			strstr(ClassName, "tunneldweller") ||
			strstr(ClassName, "npcplayertest") ||
			strstr(ClassName, "scientistnpc_cargo") ||
			strstr(ClassName, "scientistnpc_cargo_turret_any") ||
			strstr(ClassName, "scientistnpc_excavator") ||
			strstr(ClassName, "scientistnpc_patrol") ||
			strstr(ClassName, "scientistnpc_roam") ||
			strstr(ClassName, "scientistnpc_roamtethered") ||
			strstr(ClassName, "scientistnpc_excavator") ||
			strstr(ClassName, "scientistnpc_oilrig") ||
			strstr(ClassName, "heavyscientistad") ||
			strstr(ClassName, "scientist_junkpile_pistol") ||
			strstr(ClassName, "scientist_full_lr300") ||
			strstr(ClassName, "scientist_full_shotgun") ||
			strstr(ClassName, "scientist_full_pistol") ||
			strstr(ClassName, "scientist_full_pistol") ||
			strstr(ClassName, "scientist_full_any") ||
			strstr(ClassName, "scientist_full_mp5") ||
			strstr(ClassName, "scientistnpc_junkpile_pistol") ||
			strstr(ClassName, "scientistnpc_full_lr300") ||
			strstr(ClassName, "scientistnpc_full_shotgun") ||
			strstr(ClassName, "scientistnpc_full_pistol") ||
			strstr(ClassName, "scientistnpc_full_pistol") ||
			strstr(ClassName, "scientistnpc_full_any") ||
			strstr(ClassName, "scientistnpc_full_mp5") ||
			strstr(ClassName, "scientiststationary") ||
			strstr(ClassName, "scientistjunkpile") ||
			strstr(ClassName, "scientist_gunner") ||
			strstr(ClassName, "scarecrow")) {
			player.ObjectAddress = address;
			player.ObjectType = NPC;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "stone-ore.prefab") || strstr(ClassName, "ore_stone.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = _Stone;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "sulfur-ore.prefab") || strstr(ClassName, "ore_sulfur.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = _Sulfur;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "metal-ore.prefab") || strstr(ClassName, "ore_metal.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = _Metal;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "hemp-collectable.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = _Hemp;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "mushroom")) {
			player.ObjectAddress = address;
			player.ObjectType = _Mushroom;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "foodbox")) {
			player.ObjectAddress = address;
			player.ObjectType = _FoodBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "pumpkin-collectable")) {
			player.ObjectAddress = address;
			player.ObjectType = _Pumpkin;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "corn-collectable")) {
			player.ObjectAddress = address;
			player.ObjectType = _Corn;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "potato-collectable")) {
			player.ObjectAddress = address;
			player.ObjectType = _Potato;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "autoturret_deployed")) {
			player.ObjectAddress = address;
			player.ObjectType = _AutoTurret;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "sam_site_turret_deployed")) {
			player.ObjectAddress = address;
			player.ObjectType = _NavalGun;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "bradleyapc")) {
			player.ObjectAddress = address;
			player.ObjectType = _Tank;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "bradley_crate")) {
			player.ObjectAddress = address;
			player.ObjectType = _TankBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "patrolhelicopter.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = _ArmedHelicopter;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "heli_crate")) {
			player.ObjectAddress = address;
			player.ObjectType = _HelicopterBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "crate_elite")) {
			player.ObjectAddress = address;
			player.ObjectType = _NiceBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "crate_normal_2.prefab") || 
				 strstr(ClassName, "crate_basic") || 
				 strstr(ClassName, "crate_normal_2_food") || 
				 strstr(ClassName, "crate_mine") || 
				 strstr(ClassName, "crate_underwater_basic") ||
				 strstr(ClassName, "crate_normal_2_medical.prefab")  ||
				 strstr(ClassName, "crate_basic.prefab")
			){
			player.ObjectAddress = address;
			player.ObjectType = _SupplyBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "crate_tools")) {
			player.ObjectAddress = address;
			player.ObjectType = _ToolBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "crate_normal.prefab") || 
				strstr(ClassName, "crate_underwater_advanced")) {
			player.ObjectAddress = address;
			player.ObjectType = _MilitaryBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "untrap.deployed")) {
			player.ObjectAddress = address;
			player.ObjectType = _ShotgunTrap;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "cupboard.tool.deployed.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = _TerritoryBox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "codelockedhackablecrate.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = lockArop;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "supply_drop.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = SmokeArop;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "small_stash_deployed.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = hidebox;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "rhib.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = launch;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "rowboat.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = rowboat;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "submarineduo.entity.prefab") || strstr(ClassName, "submarinesolo.entity.prefab")) {
			player.ObjectAddress = address;
			player.ObjectType = submarine;
			ObjectArray.push_back(player);
		}
		else if (strstr(ClassName, "assets/rust.ai/agents/")) {
			if (strstr(ClassName, "wolf")) {
				player.ObjectAddress = address;
				player.ObjectType = _wolf;
				ObjectArray.push_back(player);
			}
			if (strstr(ClassName, "chicken")) {
				player.ObjectAddress = address;
				player.ObjectType = _chicken;
				ObjectArray.push_back(player);
			}
			if (strstr(ClassName, "simpleshark")) {
				player.ObjectAddress = address;
				player.ObjectType = _simpleshark;
				ObjectArray.push_back(player);
			}
			if (strstr(ClassName, "boar")) {
				player.ObjectAddress = address;
				player.ObjectType = _boar;
				ObjectArray.push_back(player);
			}
			if (strstr(ClassName, "bear")) {
				player.ObjectAddress = address;
				player.ObjectType = _bear;
				ObjectArray.push_back(player);
			}
			if (strstr(ClassName, "stag")) {
				player.ObjectAddress = address;
				player.ObjectType = _stag;
				ObjectArray.push_back(player);
			}
		}
	}
	Mtx.lock();
	AllObjectList.clear();
	AllObjectList = ObjectArray;
	Mtx.unlock();
	if (localExit) LocalValid = true; else LocalValid = false;
}
VOID Object::DrawEntity() {
	LastDistance = 999999;
	FLOAT IsFullScreen = 0;
	if (miscs::get().CheckFullscreen(Draw::get().GameHwnd)) IsFullScreen = 0.f; else IsFullScreen = 1.f;
	if (LocalValid) {
		if (RanderSwith) {
			Draw::get().DrawRectFilled(Draw::get().WindowWidth * 0.826, 47, Draw::get().WindowWidth / 6.4f, Draw::get().WindowWidth / 6.4f, RGBA(0.f, 0.f, 0.f, 100.f));
			Draw::get().DrawRect(Draw::get().WindowWidth * 0.826 - 2, 45, Draw::get().WindowWidth / 6.4 + 2, Draw::get().WindowWidth / 6.4 + 2, RGBA(255, 255, 255, 255), 2);
			Draw::get().DrawCircle(Draw::get().WindowWidth * 0.826f + Draw::get().WindowWidth / 6.4f / 2.f, 47.f + Draw::get().WindowWidth / 6.4f / 2.f, Draw::get().WindowWidth / 6.4f / 3.5f, RGBA(255, 255, 255, 255), 1);
			Draw::get().DrawLine(Draw::get().WindowWidth * 0.826, 47 + Draw::get().WindowWidth / 6.4 / 2, Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4, 47 + Draw::get().WindowWidth / 6.4 / 2, RGBA(255, 255, 255, 150), 1);
			Draw::get().DrawLine(Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 / 2, 47, Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 / 2, 47 + Draw::get().WindowWidth / 6.4, RGBA(255, 255, 255, 150), 1);
			Draw::get().DrawString(Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 / 2.2, 47 + Draw::get().WindowWidth / 6.4 / 2 - Draw::get().WindowWidth / 6.4 / 2.9, "100m", RGBA(255, 255, 255, 255), FALSE);
		}
		if (AimBot) {
			Draw::get().DrawCircle(Draw::get().WindowWidth / 2.f, Draw::get().WindowHeight / 2.f + IsFullScreen * 12.f, Draw::get()._x + 35, RGBA(255, 255, 255, 255), 1.f);
			Draw::get().DrawCircleFilled(Draw::get().WindowWidth / 2, Draw::get().WindowHeight / 2 + IsFullScreen * 12.f,1.5f,RGBA(255,0,0,255));
		}
		Mtx.lock();
		for (auto i = 0; i < AllObjectList.size(); i++) {
			DWORD64 a, b, c, d, e;
			DWORD64 MyInput = 0;
			Vec2 MyMouse {};
			Vec2 RadarPos{};
			Process::get().Read<DWORD64>(AllObjectList[i].ObjectAddress + Offsets::pObjectPos->a, &a, sizeof(DWORD64));
			Process::get().Read<DWORD64>(a + Offsets::pObjectPos->b, &b, sizeof(DWORD64));
			Process::get().Read<DWORD64>(b + Offsets::pObjectPos->c, &c, sizeof(DWORD64));
			Process::get().Read<DWORD64>(c + Offsets::pObjectPos->d, &d, sizeof(DWORD64));
			Process::get().Read<DWORD64>(d + Offsets::pObjectPos->e, &e, sizeof(DWORD64));
			Process::get().Read<Vec3>(e + Offsets::pObjectPos->f, &this->get().pDrawList->pos.WorldPos, sizeof(Vec3));
			if (this->LocalPlayerAddress) {
				Process::get().Read<DWORD64>(this->LocalPlayerAddress + Offsets::PlayInput, &MyInput, sizeof(DWORD64));
				Process::get().Read<Vec2>(MyInput + 0x40, &MyMouse, sizeof(Vec2));
				RadarPos = Algorithm::get().RadarCalc(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos, 120, MyMouse.x);
				RadarPos.Set(RadarPos.x + Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 / 2, 7.f + RadarPos.y + 40 + Draw::get().WindowWidth / 6.4 / 2);
			}
			switch (AllObjectList[i].ObjectType)
			{
			case LocalPlayer: {
				LocalPlayerAddress = AllObjectList[i].ObjectAddress;
				MyPos = AllObjectList[i].LocalPlayer.pos;
				break;
			}
			case Player: {
				if (Box) {
					DWORD64 f, Inventory, Belt, ItemList, Items, weapon,
					ItemDefinition, TranslatePhrase, addr_name, team = 0;
					BOOL IsSee, Dead = FALSE;
					INT WeaponUID = 0, UID = 0, Count = 0;
					FLOAT health = 0;
					std::wstring PlayerName = { };
					std::wstring WeaponName = { };
					NameList str;
					CHAR flag = 0, realname[100]{ 0 }, realweaponname[100]{ 0 };
					Vec4 Rect{ };
					Process::get().Read<CHAR>(AllObjectList[i].ObjectAddress + Offsets::Flag, &flag, sizeof(CHAR));
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					Process::get().Read<DWORD64>(AllObjectList[i].ObjectAddress + Offsets::Team, &team, sizeof(DWORD64));
					Process::get().Read<INT>(AllObjectList[i].ObjectAddress + Offsets::pActiveWeaponOffsets->a, &WeaponUID, sizeof(INT)); if (!WeaponUID)  WeaponName = L"";
					Process::get().Read<DWORD64>(AllObjectList[i].ObjectAddress + Offsets::pActiveWeaponOffsets->b, &Inventory, sizeof(DWORD64));
					Process::get().Read<DWORD64>(Inventory + Offsets::pActiveWeaponOffsets->c, &Belt, sizeof(DWORD64));
					Process::get().Read<DWORD64>(Belt + Offsets::pActiveWeaponOffsets->d, &ItemList, sizeof(DWORD64));
					Process::get().Read<DWORD64>(ItemList + 0x10, &Items, sizeof(DWORD64));
					Process::get().Read<INT>(ItemList + 0x18, &Count, sizeof(INT));
					Process::get().Read<DWORD64>(AllObjectList[i].ObjectAddress + Offsets::PlayerModel, &f, sizeof(DWORD64));
					Process::get().Read<BOOL>(f + Offsets::IsSee, &IsSee, sizeof(BOOL));
					Process::get().Read<BOOL>(AllObjectList[i].ObjectAddress + Offsets::IsDead, &Dead, sizeof(BOOL));
					for (auto i = 0; i < Count; i++)
					{
						Process::get().Read<DWORD64>(Items + Offsets::pActiveWeaponOffsets->f + (i * 0x8), &weapon, sizeof(DWORD64));
						Process::get().Read<INT>(weapon + Offsets::pActiveWeaponOffsets->e, &UID, sizeof(INT));
						if (UID == WeaponUID)
						{
							Process::get().Read<DWORD64>(weapon + Offsets::pActiveWeaponOffsets->f, &ItemDefinition, sizeof(DWORD64));
							Process::get().Read<DWORD64>(ItemDefinition + Offsets::pActiveWeaponOffsets->e, &TranslatePhrase, sizeof(DWORD64));
							Process::get().Read<DWORD64>(TranslatePhrase + 0x18, &addr_name, sizeof(DWORD64));
							Process::get().Read<NameList>(addr_name, &str, sizeof(NameList));
							if (!str.len) WeaponName = L"";
							for (auto i = 0; i < str.len && i < 20; i++)
								WeaponName += str.name[i];
							miscs::get().UnicodeToAnsi(WeaponName.c_str(), realweaponname);
						}
					}
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistancePlayer) {
							if (Dead == FALSE && flag != 16 && RanderSwith && __Box) {
								if (NowMyTeam == 0) {

									if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
										Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 0, 0, 255));
								}
								else if(team != NowMyTeam) {
									if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y > 40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
										Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 0, 0, 255));
								}
							}
						if (NowMyTeam == 0) {
							if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, Rect,this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
								if (AimBot && flag != 16) {
									if (LastDistance > Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos)) {
										if (!IsMove) TargetAddress = AllObjectList[i].ObjectAddress;
										LastDistance = Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos);
									}
									if (Deaded == TRUE || Flag == 64)
									{
										if(IsMove && Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos) >= 0.f && Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos) <= 200.f)
											TargetAddress = AllObjectList[i].ObjectAddress;
									}
								}
								Process::get().Read<DWORD64>(AllObjectList[i].ObjectAddress + Offsets::PlayerName, &d, sizeof(DWORD64));
								Process::get().Read<NameList>(d, &this->get().pDrawList->list, sizeof(NameList));
								for (auto i = 0; i < this->get().pDrawList->list.len && i < 20; i++)
									PlayerName += this->get().pDrawList->list.name[i];
								miscs::get().UnicodeToAnsi(PlayerName.c_str(), realname);
								if (flag != 16) {
									if (Dead == FALSE) {
										if (IsSee) Draw::get().DrawCircleFilled(Rect.x, Rect.y + 7.0f + IsFullScreen * 15.f - 30.0f, 5, RGBA(0, 255, 0, 255));
										else Draw::get().DrawCircleFilled(Rect.x, Rect.y + 7.0f + IsFullScreen * 15.f - 30.0f, 5, RGBA(255, 0, 0, 255));
										if (Name) Draw::get().DrawString(Rect.x + 10.0f, Rect.y + IsFullScreen * 15.f - 30.0f,realname, Setting::get().ColorPlayer, TRUE);
										if (HeldWeapon) Draw::get().DrawString(Rect.x, Rect.y + IsFullScreen * 15.f - 15.0f, miscs::get().GetActiveWeapon(realweaponname), RGBA(255, 255, 255, 255), TRUE);
										Draw::get().DrawRect(Rect.x, Rect.y + IsFullScreen * 15.f, Rect.w, Rect.h,Setting::get().ColorBox, 1);
										//Draw::get().DrawRectFilled(Rect.x + 1, Rect.y + IsFullScreen * 15.f + 1, Rect.w - 1, Rect.h - 1, RGBA(255, 255, 255, 50));
										Draw::get().DrawString(Rect.x, Rect.y + Rect.h + IsFullScreen * 15.f + 3.f, ("[血量 " + std::to_string((int)health) + "  " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorPlayer, TRUE);
										if (Ray && !IsMove) if (Rect.x > 0 && Rect.y > 0 && Rect.x < Draw::get().WindowWidth && Rect.y < Draw::get().WindowHeight)
											Draw::get().DrawLine(Draw::get().WindowWidth / 2, 20.0f, Rect.x, Rect.y + 1.5f + IsFullScreen * 15.f - 30.0f, Setting::get().ColorRay, 1);
										if (IsMove && TargetAddress == AllObjectList[i].ObjectAddress) {
											if (Point.x > -1 && Point.y > -1 && Point.x < Draw::get().WindowWidth && Point.y < Draw::get().WindowHeight)
												Draw::get().DrawLine(Draw::get().WindowWidth / 2, 20.0f, Rect.x, Rect.y + 1.5f + IsFullScreen * 15.f - 30.0f, RGBA(255, 0, 0, 255), 1);
										}
									}
								}
								else if (flag == 16 && Offline) {
									Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y + IsFullScreen * 15.f, realname, Setting::get().ColorOffline, TRUE);
									Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y + 15.0 + IsFullScreen * 15.f, ("[离线玩家 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorOffline, TRUE);
								}
							}
						}
						else {
							if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, Rect,this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
								if (AimBot && flag != 16 && team != NowMyTeam) {
									if (LastDistance > Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos)) {
										if (!IsMove) TargetAddress = AllObjectList[i].ObjectAddress;
										LastDistance = Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos);
									}
									if (Deaded == TRUE || Flag == 64)
									{
										if (IsMove && Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos) >= 0.f && Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos) <= 200.f)
											TargetAddress = AllObjectList[i].ObjectAddress;
									}
								}
								Process::get().Read<DWORD64>(AllObjectList[i].ObjectAddress + Offsets::PlayerName, &d, sizeof(DWORD64));
								Process::get().Read<NameList>(d, &this->get().pDrawList->list, sizeof(NameList));
								for (auto i = 0; i < this->get().pDrawList->list.len && i < 20; i++)
									PlayerName += this->get().pDrawList->list.name[i];
								miscs::get().UnicodeToAnsi(PlayerName.c_str(), realname);
								if (flag != 16 && team != NowMyTeam) {
									if (Dead == FALSE) {
										if (IsSee) Draw::get().DrawCircleFilled(Rect.x, Rect.y + 7.0f + IsFullScreen * 15.f - 30.0f, 5, RGBA(0, 255, 0, 255));
										else Draw::get().DrawCircleFilled(Rect.x, Rect.y + 7.0f + IsFullScreen * 15.f - 30.0f, 5, RGBA(255, 0, 0, 255));
										if (Name) Draw::get().DrawString(Rect.x + 10.0f, Rect.y + IsFullScreen * 15.f - 30.0f, realname, Setting::get().ColorPlayer, TRUE);
										if (HeldWeapon) Draw::get().DrawString(Rect.x, Rect.y + IsFullScreen * 15.f - 15.0f, miscs::get().GetActiveWeapon(realweaponname), RGBA(255, 255, 255, 255), TRUE);
										Draw::get().DrawRect(Rect.x, Rect.y + IsFullScreen * 15.f, Rect.w, Rect.h, Setting::get().ColorBox, 1);
										//Draw::get().DrawRectFilled(Rect.x + 1, Rect.y + IsFullScreen * 15.f + 1, Rect.w - 1, Rect.h - 1, RGBA(255, 255, 255, 50));
										Draw::get().DrawString(Rect.x, Rect.y + Rect.h + IsFullScreen * 15.f + 3.f, ("[血量 " + std::to_string((int)health) + "  " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorPlayer, TRUE);
										if (Ray && !IsMove) if (Rect.x > 0 && Rect.y > 0 && Rect.x < Draw::get().WindowWidth && Rect.y < Draw::get().WindowHeight)
											Draw::get().DrawLine(Draw::get().WindowWidth / 2, 20.0f, Rect.x, Rect.y + 1.5f + IsFullScreen * 15.f - 30.0f, Setting::get().ColorRay, 1);
										if (IsMove && TargetAddress == AllObjectList[i].ObjectAddress) {
											if (Point.x > -1 && Point.y > -1 && Point.x < Draw::get().WindowWidth && Point.y < Draw::get().WindowHeight)
												Draw::get().DrawLine(Draw::get().WindowWidth / 2, 20.0f, Rect.x, Rect.y + 1.5f + IsFullScreen * 15.f - 30.0f, RGBA(255, 0, 0, 255), 1);
										}
									}
								}
								else if (flag == 16 && Offline) {
									Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y + IsFullScreen * 15.f, realname, Setting::get().ColorOffline, TRUE);
									Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y + 15.0 + IsFullScreen * 15.f, ("[离线玩家 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorOffline, TRUE);
								}
							}
						}
					}
				}
				break;
			}
			case PlayerCorpse: {
				if (Corpse) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceCorpse) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[尸体包裹 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorCorpse, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Corpse && this->get().pDrawList->dis < Setting::get().DistanceCorpse) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorCorpse);
				}
				break;
			}
			case NPC: {
				FLOAT health = 0;
				if (Npc) {
					Vec4 Rect{};
					BOOL Dead;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					Process::get().Read<BOOL>(TargetAddress + Offsets::IsDead, &Dead, sizeof(BOOL));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceNPC && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, Rect, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							if (AimBot && !Dead) {
								if (LastDistance > Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos)) {
									if (!IsMove) TargetAddress = AllObjectList[i].ObjectAddress;
									LastDistance = Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos);
								}
								if (Dead == TRUE && IsMove && Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos) >= 0.0f && Aimbot::get().CalcCollimationtoTarget(this->get().pDrawList->pos.ScreenPos) <= 200.f)
									TargetAddress = AllObjectList[i].ObjectAddress;
							}
							Draw::get().DrawString(Rect.x, Rect.y + IsFullScreen * 15.f - 15.f, "NPC", Setting::get().ColorNPC, TRUE);
							Draw::get().DrawRect(Rect.x, Rect.y + IsFullScreen * 15.f, Rect.w, Rect.h, Setting::get().ColorBox, 1);
							Draw::get().DrawString(Rect.x, Rect.y + Rect.h + IsFullScreen * 15.f + 3.f, ("[血量 " + std::to_string((int)health) + "  " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorNPC, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __npc && this->get().pDrawList->dis < 300.f && health > 0) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorNPC);
				}
				break;
			}
			case _Stone: {
				if (!IsMove && Stone) {
					if (this->get().pDrawList->dis < Setting::get().DistanceStone) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[石头 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorStone, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Stone && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorStone);
				}
				break;
			}
			case _Sulfur: {
				if (!IsMove && Sulfur) {
					if (this->get().pDrawList->dis < Setting::get().DistanceSulfur) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[硫磺 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorSulfur, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Sulfur && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorSulfur);
				}
				break;
			}
			case _Metal: {
				if (!IsMove && Metal) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceMetal) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[金属 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorMetal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Metal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorMetal);
				}
				break;
			}
			case _Hemp: {
				if (!IsMove && Hemp) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceHemp) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[大麻 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorHemp, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Hemp && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorHemp);
				}
				break;
			}
			case _Mushroom: {
				if (!IsMove && Food) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceMushroom) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,  Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[蘑菇 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorMushroom, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Food && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorMushroom);
				}
				break;
			}
			case _FoodBox: {
				if (!IsMove && Food) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceFoodBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[食品盒 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorFoodBox, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Food && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorFoodBox);
				}
				break;
			}
			case _Pumpkin: {
				if (!IsMove && Food) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistancePumpkin) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[南瓜 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorPumpkin, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Food && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorPumpkin);
				}
				break;
			}
			case _Corn: {
				if (!IsMove && Food) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceCorn) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[玉米 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorCorn, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Food && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorCorn);
				}
				break;
			}
			case _Potato: {
				if (!IsMove && Food) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistancePotato) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[土豆 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().Colorpotato, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Food && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().Colorpotato);
				}
				break;
			}
			case _AutoTurret: {
				if (!IsMove && Turret) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAutoTurret) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[自动炮塔 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorTurret, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Turret && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorTurret);
				}
				break;
			}
			case _NavalGun: {
				if (!IsMove && Turret) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceNavalGun) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[防空炮塔 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAntiaircraftGun, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Turret && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAntiaircraftGun);
				}
				break;
			}
			case _Tank: {
				if (Tank) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
						Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[武装坦克 血量 " + std::to_string((int)health) + "  " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorTank, TRUE);
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Tank && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorTank);
				}
				break;
			}
			case _TankBox: {
				if (!IsMove && Superbox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceNiceBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[坦克-超级军工箱 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorSuperbox, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Superbox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorSuperbox);
				}
				break;
			}
			case _ArmedHelicopter: {
				if (ArmedHelicopter) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
						Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[武装直升机 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorArmedHelicopter, TRUE);
					}
				}
				break;
			}
			case _HelicopterBox: {
				if (!IsMove && Superbox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceNiceBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[武直-超级军工箱 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorSuperbox, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Superbox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorSuperbox);
				}
				break;
			}
			case _NiceBox: {
				if (!IsMove && Superbox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceNiceBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[超级军工箱 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorSuperbox, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Superbox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorSuperbox);
				}
				break;
			}
			case _SupplyBox: {
				if (!IsMove && SupplyBox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceSupplyBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[补给箱 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorSupplyBox, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __SupplyBox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorSupplyBox);
				}
				break;
			}
			case _ToolBox: {
				if (!IsMove && SupplyBox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceToolBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[工具箱 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorToolBox, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __SupplyBox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorToolBox);
				}
				break;
			}
			case _MilitaryBox: {
				if (!IsMove && MilitaryBox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceMilitaryBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[军工箱 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorMilitaryBox, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __MilitaryBox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorMilitaryBox);
				}
				break;
			}
			case _ShotgunTrap: {
				if (!IsMove && Turret) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceShotgunTrap) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[散弹枪陷阱 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorTrap, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Turret && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorTrap);
				}
				break;
			}
			case _TerritoryBox: {
				if (!IsMove && TerritoryBox) {
					DWORD64 f ,g,h,j = 0;
					INT count = 0,k = 0;
					Process::get().Read<DWORD64>(AllObjectList[i].ObjectAddress + Offsets::authorizedPlayers, &f, sizeof(DWORD64));
					Process::get().Read<INT>(f + 0x18, &count, sizeof(INT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceTerritoryBox) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[领地柜 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorTerritoryBox, TRUE);
							for (auto i = 0; i < count; i++) {
								WCHAR PlayerName[100] = { 0 };
								CHAR realname[100] = { 0 };
								Process::get().Read<DWORD64>(f + 0x10, &g, sizeof(DWORD64));
								Process::get().Read<DWORD64>(g + 0x20 + i * 8, &h, sizeof(DWORD64));
								Process::get().Read<DWORD64>(h + 0x18, &j, sizeof(DWORD64));
								Process::get().Read<INT>(j + 0x10, &k, sizeof(INT));
								k *= 2;
								Process::get().Read<WCHAR>(j + 0x14, PlayerName,k);
								miscs::get().UnicodeToAnsi(PlayerName, realname);
								Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y - ((i + 1) * 16), realname, Setting::get().ColorTerritoryBox, TRUE);
							}
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __TerritoryBox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorTerritoryBox);
				}
				break;
			}
			case _wolf: {
				if (!IsMove && Animal) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAnimal && health > 0.0f) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[狼 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAnimal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Animal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAnimal);
				}
				break;
			}
			case _chicken: {
				if (!IsMove && Animal) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[鸡 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAnimal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Animal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAnimal);
				}
				break;
			}
			case _simpleshark: {
				if (!IsMove && Animal) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos,Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[鲨鱼 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAnimal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Animal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAnimal);
				}
				break;
			}
			case _boar: {
				if (!IsMove && Animal) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAnimal && health >0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[猪 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAnimal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Animal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAnimal);
				}
				break;
			}
			case _bear: {
				if (!IsMove && Animal) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[熊 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAnimal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Animal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAnimal);
				}
				break;
			}
			case _stag: {
				if (!IsMove && Animal) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[鹿 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAnimal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Animal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAnimal);
				}
				break;
			}
			case _horse: {
				if (!IsMove && Animal) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[马 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), Setting::get().ColorAnimal, TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Animal && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, Setting::get().ColorAnimal);
				}
				break;
			}
			case lockArop: {
				if (!IsMove && AropBox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
						Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[密码锁空投 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), RGBA(255, 165, 0,255), TRUE);
					}
				}
				if (LocalPlayerAddress && RanderSwith && __AropBox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 165, 0, 255));
				}
				break;
			}
			case SmokeArop: {
				if (!IsMove && AropBox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
						Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[空投 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), RGBA(255, 165, 0, 255), TRUE);
					}
				}
				if (LocalPlayerAddress && RanderSwith && __AropBox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 165, 0, 255));
				}
				break;
			}
			case hidebox: {
				if (!IsMove && HideBox) {
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < 350.f) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[小藏匿 " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), RGBA(255, 69, 0, 255), TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __HideBox && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 69, 0, 255));
				}
				break;
			}
			case launch: {
				if (!IsMove && Car) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < 2000.f && Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[汽艇 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), RGBA(255, 255, 255, 255), TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Car && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 255, 255, 255));
				}
				break;
			}
			case rowboat: {
				if (!IsMove && Car) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < 2000.f && Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[划艇 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), RGBA(255, 255, 255, 255), TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Car && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 255, 255, 255));
				}
				break;
			}
			case submarine: {
				if (!IsMove && Car) {
					FLOAT health = 0;
					Process::get().Read<FLOAT>(AllObjectList[i].ObjectAddress + Offsets::ObjectHealth, &health, sizeof(FLOAT));
					this->get().pDrawList->dis = Algorithm::get().Get3Ddistance(AllObjectList[i].LocalPlayer.pos, this->get().pDrawList->pos.WorldPos);
					if (this->get().pDrawList->dis < 2000.f && Setting::get().DistanceAnimal && health > 0) {
						if (Algorithm::get().WorldToScreen(Matrix, this->get().pDrawList->pos.WorldPos, this->get().pDrawList->pos.ScreenPos, Draw::get().WindowWidth, Draw::get().WindowHeight)) {
							Draw::get().DrawString(this->get().pDrawList->pos.ScreenPos.x, this->get().pDrawList->pos.ScreenPos.y, ("[潜艇 血量 " + std::to_string((int)health) + " " + std::to_string((int)this->get().pDrawList->dis) + "m]").c_str(), RGBA(255, 255, 255, 255), TRUE);
						}
					}
				}
				if (LocalPlayerAddress && RanderSwith && __Car && this->get().pDrawList->dis < 300.f) {
					if (RadarPos.x > Draw::get().WindowWidth * 0.826 && RadarPos.x < Draw::get().WindowWidth * 0.826 + Draw::get().WindowWidth / 6.4 && RadarPos.y>40 && RadarPos.y < Draw::get().WindowWidth / 6.4 + 40)
						Draw::get().DrawCircleFilled(RadarPos.x, RadarPos.y, 2, RGBA(255, 255, 255, 255));
				}
				break;
			}
			}
		}
		Mtx.unlock();
	}
}