#include "global.h"
BOOL HideMenu, AimBot, Position, swag, Corpse,Recoil, Npc, Stone, Sulfur, Metal, NoDrop, Wall, Nofall, fly, Ishold, Melee, AropBox,
Hemp, Food, Turret, Tank, Animal, ArmedHelicopter, Car, Superbox, SupplyBox, MilitaryBox, Admin, TerritoryBox, Offline, IsMove, MagicBullet = FALSE;
BOOL Box = TRUE, Ray = TRUE, IsExit = TRUE, AimBotType = TRUE;
BOOL RanderSwith = TRUE, __Box = TRUE;
BOOL __Corpse, __npc, __MilitaryBox, __Stone, __Sulfur, __Metal, __AropBox,
__Hemp, __Food, __Turret, __Tank, __Animal, __Car, __Superbox, __HideBox, __SupplyBox, __TerritoryBox, __Offline;
BOOL Name = TRUE, HeldWeapon = TRUE, HideBox = FALSE;
DWORD MenuOption = 1;
Vec2 Point{ 1,1 };
FLOAT Down = 0, DisPos = 1.32f, WindowX = 50.f, WindowY = 50.f;
FLOAT Matrix[4][4] = { 0 };
DWORD64 Addressa = 0;
INT BonePosition = 79;
Vec3 EnemyPos{ };
DWORD64 v = 0;