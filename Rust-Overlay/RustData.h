#pragma once
#include <Windows.h>
#include <Vector>
#include <emmintrin.h>
#include <mutex>
#include "misc.h"
#include "AimBot.h"
#include "Struct.h"
#include "Settings.h"
namespace Offsets {
	static DWORD64 BaseNetworkableOffset = 0x3171BE0;
	static DWORD64 GameObjectManagerOffset = 0x3172AB0;
	static DWORD64 TrackHook = 0x00;
	static DWORD64 TrackJMPHook = 0x00;
	static DWORD Fly = 0x291A3C4;
	static DWORD Nofall = 0x2647CF8;
	static DWORD ObjectCount = 0x10;
	static DWORD ObjectList = 0x18;
	static DWORD ObjectHealth = 0x224; //protected float _health
	static DWORD PlayerName = 0x6D8;//protected string _displayName
	static DWORD PlayerModel = 0x4C0;//PlayerModel
	static DWORD IsSee = 0x288;//internal bool visible
	static DWORD MyTeam = 0x598;//public PlayerTeam clientTeam
	static DWORD MyNowTeam = 0x18;
	static DWORD Team = 0x590;//public ulong currentTeam
	static DWORD IsDead = 0x21C;//public BaseCombatEntity.LifeState lifestate
	static DWORD object = 0x28;
	static DWORD Flag = 0x678;//public BasePlayer.PlayerFlags playerFlags
	static DWORD PlayInput = 0x4E0;//public PlayerInput input
	static DWORD IsOpenMenu = 0x98;
	static DWORD Angles = 0x3C;//private Vector3 bodyAngles
	static DWORD RecoilAngles = 0x64;//public Vector3 recoilAngles
	static DWORD GroundAngle = 0xC4;//private float groundAngle
	static DWORD GroundAngleNew = 0xC8;//private float groundAngleNew
	static DWORD Velocity = 0x23C;//newVelocity
	static DWORD BaseMovement = 0x4E8;//public BaseMovement movement
	static DWORD ModelState = 0x210;//private ModelState modelState
	static DWORD ModelFlag = 0x24;
	static DWORD authorizedPlayers = 0x590;//public List<PlayerNameID> authorizedPlayers
	static DWORD Eyes = 0x680;//public PlayerEyes eyes
	static DWORD HeldEntity = 0x98;//private EntityRef heldEntity
	static DWORD StancePenalty = 0x31C;//private float stancePenalty
	static DWORD AimconePenalty = 0x320;//private float aimconePenalty
	static DWORD AimCone = 0x2E8;//public float aimCone
	static DWORD HipAimCone = 0x2EC;//public float hipAimCone
	static DWORD AimconePenaltyPerShot = 0x2F0;//public float aimconePenaltyPerShot
	static DWORD aiStrikeDelay = 0x2AC; //public float aiStrikeDelay
	static DWORD maxDistance = 0x290;//public float maxDistance
	static DWORD AttackRadius = 0x294;//public float attackRadius
	static DWORD BlockSprintOnAttack = 0x299; //public bool blockSprintOnAttack
	static DWORD onlyThrowAsProjectile = 0x27A;//public bool onlyThrowAsProjectile
	static DWORD isAutomatic = 0x298;//public bool isAutomatic
	static DWORD throwReady = 0x2D0; //public bool throwReady
	static DWORD clothingAccuracyBonus = 0x754;//public float clothingAccuracyBonus
	static DWORD playerColliderDucked = 0x6FC;//private BasePlayer.CapsuleColliderInfo playerColliderDucked
	static DWORD waterLevel = 0x6FC;//public float waterLevel
	static DWORD gravityTestRadius = 0x80;//public float gravityTestRadius
	static DWORD capsuleHeight = 0x68;//public float capsuleHeight
	static DWORD capsuleCenter = 0x6C;//public float capsuleCenter
	static DWORD gravityMultiplier = 0x84;//public float gravityMultiplier
	static DWORD HookTrack = 0x2709D0;
	static DWORD SendProjectileAttack = 0x579A50;
	static DWORD AimCones = 0x9237BA;
	static DWORD modifier = 0x284;//public Projectile.Modifier modifier
	static DWORD primaryMagazine = 0x2B8;//public BaseProjectile.Magazine primaryMagazine;
	static struct BaseOffsets {
		DWORD a = 0xB8;
		DWORD b = 0x0;
		DWORD c = 0x10;
		DWORD d = 0x28;
	};
	static struct MatrixOffsets {
		DWORD a = 0xB8;
		DWORD b = 0x0;
		DWORD c = 0x10;
		DWORD d = 0x2E4;
	};
	static struct EntityListOffsets {
		DWORD a = 0x10;
		DWORD b = 0x30;
		DWORD c = 0x60;
	};
	static struct ObjectPosOffsets {
		DWORD a = 0x10;
		DWORD b = 0x30;
		DWORD c = 0x30;
		DWORD d = 0x8;
		DWORD e = 0x38;
		DWORD f = 0x90;
	};
	static struct ActiveWeaponOffsets {
		DWORD a = 0x5C8;
		DWORD b = 0x688;
		DWORD c = 0x28;
		DWORD d = 0x38;
		DWORD e = 0x28;
		DWORD f = 0x20;
	};
	static BaseOffsets* pBaseOffsets = new BaseOffsets;
	static MatrixOffsets* pMatrixOffsets = new MatrixOffsets;
	static EntityListOffsets* pEntityListOffsets = new EntityListOffsets;
	static ObjectPosOffsets* pObjectPos = new ObjectPosOffsets;
	static ActiveWeaponOffsets* pActiveWeaponOffsets = new ActiveWeaponOffsets;
}
class Object : public Singleton<ObjectList> {
public:
	BOOL GetBase();
	BOOL GetObjectCount();
	VOID GetObjectEntityList();
	VOID DrawEntity();
	DWORD64 GameAssembly = NULL;
	DWORD64 UnityPlayer = NULL;
	DWORD64 LocalPlayerAddress = 0;
	bool LocalValid = false;
	std::mutex Mtx;
	BOOL AimBotThread();
	DWORD64 TrackSwitch = NULL;
	DWORD64 TrackAngleAddress = NULL;
	CHAR Flag = 0;
	BOOL Deaded;
	BOOL Cow;
private:
	DWORD64 base = NULL;
	DWORD64 BaseNetworkable = NULL;
	Vec3 MyPos{ };
	DWORD64 MyTeam;
	DWORD64 NowMyTeam;
	FLOAT TargetHealth = 0;
	DWORD64 TargetAddress = 0;
	FLOAT LastDistance = 0;
	Vec3 GetPosition(DWORD64 address, INT Bone);
};
class Entity : public ObjectMode<Object> {
protected:
	Entity() {}
	~Entity() {}
public:
	BOOL retrun = NULL;
};
