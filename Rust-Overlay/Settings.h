#pragma once
#include "Renderer.h"
#include "global.h"
#include "misc.h"
class Setting : public Singleton<Setting> {
public:
	ImVec4 ColorPlayer = RGBA(255, 255, 0, 255);
	ImVec4 ColorBox = RGBA(255, 255, 255, 255);
	ImVec4 ColorRay = RGBA(255, 255, 255, 255);
	ImVec4 ColorCorpse = RGBA(255, 0, 0, 255);
	ImVec4 ColorNPC = RGBA(255, 150, 255, 255);
	ImVec4 ColorOffline = RGBA(128, 0, 128, 255);
	ImVec4 ColorStone = RGBA(255, 255, 255, 255);
	ImVec4 ColorSulfur = RGBA(50, 205, 50, 255);
	ImVec4 ColorMetal = RGBA(218, 165, 32, 255);
	ImVec4 ColorHemp = RGBA(152, 251, 152, 255);
	ImVec4 ColorMushroom = RGBA(244, 164, 96, 255);
	ImVec4 ColorFoodBox = RGBA(60, 179, 113, 255);
	ImVec4 ColorPumpkin = RGBA(255, 165, 0, 255);
	ImVec4 ColorCorn = RGBA(255, 215, 0, 255);
	ImVec4 Colorpotato = RGBA(218, 165, 32, 255);
	ImVec4 ColorAnimal = RGBA(255, 255, 255, 255);
	ImVec4 ColorTank = RGBA(255, 0, 0, 255);
	ImVec4 ColorArmedHelicopter = RGBA(255, 0, 0, 255);
	ImVec4 ColorTurret = RGBA(0, 0, 255, 255);
	ImVec4 ColorTrap = RGBA(255, 128, 0, 255);
	ImVec4 ColorAntiaircraftGun = RGBA(0, 0, 255, 255);
	ImVec4 ColorSupplyBox = RGBA(255, 20, 147, 255);
	ImVec4 ColorToolBox = RGBA(255, 255, 255, 255);
	ImVec4 ColorMilitaryBox = RGBA(255, 165, 0, 255);
	ImVec4 ColorSuperbox = RGBA(255, 215, 0, 255);
	ImVec4 ColorTerritoryBox = RGBA(255, 140, 0, 255);
	FLOAT DistancePlayer = 400.f;
	FLOAT DistanceCorpse = 300.f;
	FLOAT DistanceNPC = 300.f;
	FLOAT DistanceStone = 400.f;
	FLOAT DistanceSulfur = 400.f;
	FLOAT DistanceMetal = 400.f;
	FLOAT DistanceHemp = 300.f;
	FLOAT DistanceMushroom = 300.f;
	FLOAT DistanceFoodBox = 300.f;
	FLOAT DistancePumpkin = 300.f;
	FLOAT DistanceCorn = 300.f;
	FLOAT DistancePotato = 300.f;
	FLOAT DistanceVehicle = 800.f;
	FLOAT DistanceAutoTurret = 300.f;
	FLOAT DistanceNavalGun = 800.f;
	FLOAT DistanceNiceBox = 350.f;
	FLOAT DistanceSupplyBox = 350.f;
	FLOAT DistanceToolBox = 350.f;
	FLOAT DistanceMilitaryBox = 350.f;
	FLOAT DistanceShotgunTrap = 300.f;
	FLOAT DistanceTerritoryBox = 400.f;
	FLOAT DistanceAnimal = 300.f;
public:
	VOID ReadRendererSettings();
	VOID ReadColorSettings();
	VOID ReadDistanceSettings();
};
VOID setting();