#pragma once
#include <stdio.h>
struct Vec2 {
	FLOAT x, y;
	Vec2() = default;
	Vec2(FLOAT _x, FLOAT _y) {
		this->x = _x;
		this->y = _y;
	}
	VOID Set(FLOAT _x, FLOAT _y) {
		this->x = _x;
		this->y = _y;
	}
	Vec2& operator-(float v) {
		x -= v; y -= v;
		return *this;
	}
	Vec2& operator+(float v) {
		x += v; y += v;
		return *this;
	}
	Vec2& operator*(float v) {
		x *= v; y *= v;
		return *this;
	}
	Vec2& operator/(float v) {
		x /= v; y /= v;
		return *this;
	}
	Vec2 operator+(const Vec2& v) {
		return Vec2(x + v.x, y + v.y);
	}
	Vec2 operator-(const Vec2& v) {
		return Vec2(x - v.x, y - v.y);
	}
	Vec2 operator*(const Vec2& v) {
		return Vec2(x * v.x, y * v.y);
	}
	Vec2 operator/(const Vec2& v) {
		return Vec2(x / v.x, y / v.y);
	}
};
struct Vec3 {
	FLOAT x, y, z;
	Vec3() = default;
	Vec3(FLOAT _x, FLOAT _y, FLOAT _z) {
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}
	VOID Set(FLOAT _x, FLOAT _y, FLOAT _z) {
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}
	Vec3& operator-(float v) {
		x -= v; y -= v; z -= v;
		return *this;
	}
	Vec3& operator+(float v) {
		x += v; y += v; z += v;
		return *this;
	}
	Vec3& operator*(float v) {
		x *= v; y *= v; z *= v;
		return *this;
	}
	Vec3& operator/(float v) {
		x /= v; y /= v; z /= v;
		return *this;
	}
	Vec3 operator+(const Vec3& v) {
		return Vec3(x + v.x, y + v.y, z + v.z);
	}
	Vec3 operator-(const Vec3& v){
		return Vec3(x - v.x, y - v.y, z - v.z);
	}
	Vec3 operator*(const Vec3& v) {
		return Vec3(x * v.x, y * v.y, z * v.z);
	}
	Vec3 operator/(const Vec3& v) {
		return Vec3(x / v.x, y / v.y, z / v.z);
	}
};
struct Vec4 {
	FLOAT x, y, w, h;
	Vec4() = default;
	Vec4(FLOAT _x, FLOAT _y, FLOAT _w, FLOAT _h) {
		this->x = _x;
		this->y = _y;
		this->w = _w;
		this->h = _h;
	}
	VOID Set(FLOAT _x, FLOAT _y, FLOAT _w, FLOAT _h) {
		this->x = _x;
		this->y = _y;
		this->w = _w;
		this->h = _h;
	}
	Vec4& operator-(float v) {
		x -= v; y -= v; w -= v; h -= v;
		return *this;
	}
	Vec4& operator+(float v) {
		x += v; y += v; w += v; h += v;
		return *this;
	}
	Vec4& operator*(float v) {
		x *= v; y *= v; w *= v; h *= v;
		return *this;
	}
	Vec4& operator/(float v) {
		x /= v; y /= v; w /= v; h /= v;
		return *this;
	}
	Vec4 operator+(const Vec4& v) {
		return Vec4(x + v.x, y + v.y, w + v.w, h + v.h);
	}
	Vec4 operator-(const Vec4& v) {
		return Vec4(x - v.x, y - v.y, w - v.w, h - v.h);
	}
	Vec4 operator*(const Vec4& v) {
		return Vec4(x * v.x, y * v.y, w * v.w, h * v.h);
	}
	Vec4 operator/(const Vec4& v) {
		return Vec4(x / v.x, y / v.y, w / v.w, h / v.h);
	}
};
enum ObjectTypeList {
	LocalPlayer,//本人
	Player,//玩家
	PlayerCorpse,//玩家尸体背包
	NPC,//科学家
	_Stone,//石头
	_Sulfur,//硫磺
	_Metal,//金属矿石
	_Hemp,//大麻
	_Mushroom,//蘑菇
	_FoodBox,//食物盒子
	_Pumpkin,//南瓜
	_Corn,//玉米
	_Potato,//土豆
	_AutoTurret,//自动炮塔
	_NavalGun,//防空炮塔
	_Tank,//坦克
	_TankBox,//坦克军工箱
	_NiceBox,//超级军工箱
	_ArmedHelicopter,// 武装直升机
	_HelicopterBox,//武装直升机军工箱
	_SupplyBox,//补给箱
	_ToolBox,//工具箱	
	_MilitaryBox,//军工箱
	_ShotgunTrap,//散弹枪陷阱
	_Airdrop,//空投
	_TerritoryBox,//领地柜
    _wolf,//狼
	_chicken,//鸡
	_simpleshark,//鲨鱼
	_boar,//猪
	_bear,//熊
	_stag,//鹿
	_horse,//马
	lockArop,//密码锁空投
	SmokeArop,//烟雾空投
	hidebox,//小藏匿
	launch,//汽艇
	rowboat,//划艇
	submarine//潜艇
};
static struct ObjectPos
{
	Vec3 WorldPos;
	Vec2 ScreenPos;
};
static struct NameList
{
	char stub[0x10] = { 0 };
	int len;
	wchar_t name[100] = { 0 };
};
static struct LocalPlayerInfo {
	DWORD64 Team;
	DWORD64 NowTeam;
	Vec3 pos;
};
static struct ObjectInfo {
	LocalPlayerInfo LocalPlayer;
	DWORD64 ObjectAddress = NULL;
	ObjectTypeList ObjectType;
};
static struct DrawList {
	ObjectPos pos;
	FLOAT dis;
	NameList list;
};
static struct ObjectList {
	DrawList* pDrawList = new DrawList;
	INT ObjectCount;
};
static struct BulletInfo {
	FLOAT ProjectileVelocity;
	FLOAT GravityModifier;
	FLOAT Drag;
	FLOAT BulletSpeed;
	BOOL IsCowBow;
};
enum BoneList : INT {
	l_hip = 1,
	l_knee,
	l_foot,
	l_toe,
	l_ankle_scale,
	pelvis,
	penis,
	GenitalCensor,
	GenitalCensor_LOD0,
	Inner_LOD0,
	GenitalCensor_LOD1,
	GenitalCensor_LOD2,
	r_hip,
	r_knee,
	r_foot,
	r_toe,
	r_ankle_scale,
	spine1,
	spine1_scale,
	spine2,
	spine3,
	spine4,
	l_clavicle,
	l_upperarm,
	l_forearm,
	l_hand,
	l_index1,
	l_index2,
	l_index3,
	l_little1,
	l_little2,
	l_little3,
	l_middle1,
	l_middle2,
	l_middle3,
	l_prop,
	l_ring1,
	l_ring2,
	l_ring3,
	l_thumb1,
	l_thumb2,
	l_thumb3,
	IKtarget_righthand_min,
	IKtarget_righthand_max,
	l_ulna,
	neck,
	head,
	jaw,
	eyeTranform,
	l_eye,
	l_Eyelid,
	r_eye,
	r_Eyelid,
	r_clavicle,
	r_upperarm,
	r_forearm,
	r_hand,
	r_index1,
	r_index2,
	r_index3,
	r_little1,
	r_little2,
	r_little3,
	r_middle1,
	r_middle2,
	r_middle3,
	r_prop,
	r_ring1,
	r_ring2,
	r_ring3,
	r_thumb1,
	r_thumb2,
	r_thumb3,
	IKtarget_lefthand_min,
	IKtarget_lefthand_max,
	r_ulna,
	l_breast,
	r_breast,
	BoobCensor,
	BreastCensor_LOD0,
	BreastCensor_LOD1,
	BreastCensor_LOD2,
	collision,
	displacement
};