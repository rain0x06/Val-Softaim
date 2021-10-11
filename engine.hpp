#pragma once
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include "structure.hpp"
#include "xxhash.hpp"


namespace Prestige
{

	namespace Engine
	{

		extern uint64_t retaddr;
		extern uint64_t GWorld;
		extern uint64_t GNames;
		extern uint64_t UWorld;
		extern uint64_t PersistentLevel;
		extern uint64_t OwningGameInstance;
		extern uint64_t LocalPlayers;
		extern uint64_t LocalPlayer;
		extern uint64_t PlayerController;
		extern uint64_t PlayerCameraManager;
		extern uint64_t ControlRotation;
		extern uint64_t AcknowledgedPawn;
		extern uint64_t Actors;
		extern uint32_t ActorCounts;
		extern uint32_t ObjectID;
		extern uint64_t CameraCache;
		extern uint64_t Mesh;
		extern uint64_t IsDormant;
		extern uint64_t StaticMesh;
		extern uint64_t ComponentToWorld;
		extern uint64_t DamageHandler;
		extern uint64_t CachedLife;
		extern uint64_t PlayerState;
		extern uint64_t TeamComponent;
		extern uint64_t Team;
		extern uint64_t PlayerName;

		extern uint8_t LocalTeam;

		extern std::vector<int> MaleIds;
		extern std::vector<int> FemaleIds;
		extern int SpyCamId, ClayBombId, SovaDroneId;

		void Init();
		void Update();

		FTransform GetBoneIndex(uint64_t mesh, int index);
		FVector GetBoneWithRotation(uint64_t mesh, int id);
		FVector WorldToScreen(FVector WorldLocation, FCameraCacheEntry CameraCacheL);

		bool IsMale(uint32_t obj_id);
		bool IsFemale(uint32_t obj_id);

		const char* GetNameFromID(int ID);

	}

}
