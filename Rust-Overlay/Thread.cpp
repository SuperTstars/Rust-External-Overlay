#include "Thread.h"
#include "Process.h"
#include "global.h"
namespace Thread {
	VOID GetData() {
		while (IsExit) {
			if (!Process::get().GetProcessId(L"RustClient.exe")) {
				IsExit = FALSE;
				miscs::get().SendNotify("Bye", "欢迎下次使用~", TRUE);
				miscs::get().DeleteNotify();
				ExitProcess(0);
			}
			if (!Entity::getobject().GetBase()) Entity::getobject().LocalValid = false;
			if (Entity::getobject().GetObjectCount()) Entity::getobject().GetObjectEntityList();else Entity::getobject().LocalValid = false;
			Sleep(1);
		}
		return;
	}
	VOID GetMatrix() {
		while (IsExit) {
			if (Entity::getobject().LocalValid) {
				DWORD64 Matrixa, Matrixb, Matrixc, Matrixd;
				Process::get().Read<DWORD64>(Entity::getobject().GameAssembly + Offsets::GameObjectManagerOffset, &Matrixa, sizeof(DWORD64));
				Process::get().Read<DWORD64>(Matrixa + Offsets::pMatrixOffsets->a, &Matrixb, sizeof(DWORD64));
				Process::get().Read<DWORD64>(Matrixb + Offsets::pMatrixOffsets->b, &Matrixc, sizeof(DWORD64));
				Process::get().Read<DWORD64>(Matrixc + Offsets::pMatrixOffsets->c, &Matrixd, sizeof(DWORD64));
				Algorithm::get().ReadMatrix(Matrixd + Offsets::pMatrixOffsets->d, Matrix);
			}
			Sleep(1);
		}
	}
	VOID DrawEntity() {
		if (Entity::getobject().LocalValid) Entity::getobject().DrawEntity();
	}
	VOID AimBotthread() {
		while (IsExit) {
			Entity::getobject().AimBotThread();
			Sleep(8);
		}
		return;
	}		
	VOID NoRecoilthread() {
		while (IsExit) {
			if (Entity::getobject().LocalValid && Ishold) {
				if (AimBotType && swag) {
					FLOAT b = 0.f;
					Process::get().Read<DWORD64>(Aimbot::get().GetWeapon(Entity::getobject().LocalPlayerAddress, FALSE) + Offsets::HeldEntity, &held, sizeof(DWORD64));
					Process::get().Write<FLOAT>(held + Offsets::StancePenalty, &b, sizeof(FLOAT));
					Process::get().Write<FLOAT>(held + Offsets::AimconePenalty, &b, sizeof(FLOAT));
					Process::get().Write<FLOAT>(held + Offsets::AimCone, &b, sizeof(FLOAT));
					Process::get().Write<FLOAT>(held + Offsets::HipAimCone, &b, sizeof(FLOAT));
					Process::get().Write<FLOAT>(held + Offsets::AimconePenaltyPerShot, &b, sizeof(FLOAT));
				}
				if (Recoil) Aimbot::get().NoRecoil(Entity::getobject().LocalPlayerAddress);
			}
		miscs::get().Sleep_(100);
		}
		return;
	}
	VOID OpenAdmin() {
		while (IsExit) {
			if (Entity::getobject().LocalValid) Aimbot::get().admin(Entity::getobject().LocalPlayerAddress);
			if (Entity::getobject().LocalValid && Melee) Aimbot::get().GeMetee(Entity::getobject().LocalPlayerAddress);
			Sleep(1000);
		}
		return;
	}
}
