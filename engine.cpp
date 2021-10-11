#include "engine.hpp"
#include "memory.hpp"
#include "d3d/renderer.hpp"

namespace Prestige
{

	uint64_t Engine::retaddr = mem::dwGameBase + 0x6965010;
	uint64_t Engine::GWorld{};
	uint64_t Engine::GNames{};
	uint64_t Engine::UWorld{};
	uint64_t Engine::PersistentLevel{}; //Engine.World.PersistentLevel
	uint64_t Engine::OwningGameInstance{}; //Engine.World.OwningGameInstance
	uint64_t Engine::LocalPlayers{}; //Engine.GameInstance.LocalPlayers
	uint64_t Engine::LocalPlayer{};
	uint64_t Engine::PlayerController{}; //Engine.Player.PlayerController
	uint64_t Engine::PlayerCameraManager{}; //Engine.PlayerController.PlayerCameraManager
	uint64_t Engine::ControlRotation = 0x418; //Engine.Controller.ControlRotation
	uint64_t Engine::AcknowledgedPawn{}; //Engine.PlayerController.AcknowledgedPawn
	uint64_t Engine::Actors{};
	uint32_t Engine::ActorCounts{};
	uint32_t Engine::ObjectID = 0x18;
	uint64_t Engine::CameraCache = 0x1EE0; //Engine.PlayerCameraManager.CameraCachePrivate
	uint64_t Engine::Mesh = 0x408; //Engine.Character.Mesh
	uint64_t Engine::IsDormant = 0x100;
	uint64_t Engine::StaticMesh = 0x548; //Engine.StaticMeshComponent.StaticMesh
	uint64_t Engine::ComponentToWorld = 0x250;
	uint64_t Engine::DamageHandler = 0xA50; //ShooterGame.ShooterCharacter.DamageHandler
	uint64_t Engine::CachedLife = 0x190; //ShooterGame.DamageableComponent.CachedLife
	uint64_t Engine::PlayerState = 0x3C8; //Engine.Pawn.PlayerState
	uint64_t Engine::TeamComponent = 0x5B0; //ShooterGame.AresPlayerStateBase.TeamComponent
	uint64_t Engine::Team = 0x118; //ShooterGame.BombTeamComponent.Team
	uint64_t Engine::PlayerName = 0x3A8; //Engine.PlayerState.PlayerName

	uint8_t Engine::LocalTeam{};

	std::vector<int> Engine::MaleIds{};
	std::vector<int> Engine::FemaleIds{};
	int Engine::SpyCamId{}, Engine::ClayBombId{}, Engine::SovaDroneId{};

	void Engine::Init()
	{
		if (MessageBoxA(NULL, "", "", MB_YESNO) == IDNO)
		{
			Engine::GWorld = mem::dwGameBase + 0x627CED8;
			Engine::GNames = mem::dwGameBase + 0x611EC00;
		}
		else //KR
		{
			Engine::GWorld = mem::dwGameBase + 0x627CED8;
			Engine::GNames = mem::dwGameBase + 0x611EC00;
		}

		for (int i = 0; i < 2000000; i++)
		{
			std::string obj_name = GetNameFromID(i);
			xxh::hash_t<64> obj_hash = xxh::xxhash<64>(obj_name);
			if (obj_hash == 0xb1af9af29258106b || obj_hash == 0xdd5422acbfe9fb66 || obj_hash == 0xfa5f8033c243c75b || obj_hash == 0x58555ab692678712 || obj_hash == 0x73885184412a945e || obj_hash == 0x7a7bac1a618b7e0b)
				MaleIds.push_back(i);
			else if (obj_hash == 0xd9b322ca8060a766 || obj_hash == 0xf01dfe329146f88c || obj_hash == 0x4949f27bf5cfdf21 || obj_hash == 0x6d7be66bc8e03486)
				FemaleIds.push_back(i);
			else if (obj_hash == 0x07726935db2f8e76)
				SpyCamId = i;
			else if (obj_hash == 0xfb793fed48d4399f)
				ClayBombId = i;
			else if (obj_hash == 0xfd2802bbf4b8dd49)
				SovaDroneId = i;
		}
	}

	void Engine::Update()
	{
		Engine::UWorld = mem::RPM<uint64_t>(Engine::GWorld);
		Engine::PersistentLevel = mem::RPM<uint64_t>(Engine::UWorld + 0x38);
		Engine::OwningGameInstance = mem::RPM<uint64_t>(Engine::UWorld + 0x190);
		Engine::LocalPlayers = mem::RPM<uint64_t>(OwningGameInstance + 0x40);
		Engine::LocalPlayer = mem::RPM<uint64_t>(LocalPlayers);
		Engine::PlayerController = mem::RPM<uint64_t>(LocalPlayer + 0x38);
		Engine::PlayerCameraManager = mem::RPM<uint64_t>(PlayerController + 0x448);
		Engine::AcknowledgedPawn = mem::RPM<uint64_t>(PlayerController + 0x430);
		Engine::Actors = mem::RPM<uint64_t>(PersistentLevel + 0xB0);
		Engine::ActorCounts = mem::RPM<uint32_t>(PersistentLevel + 0xB8);

		Engine::LocalTeam = mem::RPM<uint8_t>(mem::RPM<uint64_t>(mem::RPM<uint64_t>(AcknowledgedPawn + PlayerState) + TeamComponent) + Team);
	}

	FTransform Engine::GetBoneIndex(uint64_t mesh, int index)
	{
		uint64_t StaticMesh = mem::RPM<uint64_t>(mesh + Engine::StaticMesh);

		return mem::RPM<FTransform>(StaticMesh + (index * 0x30));
	}

	FVector Engine::GetBoneWithRotation(uint64_t mesh, int id)
	{
		FTransform Bone = GetBoneIndex(mesh, id);
		FTransform ComponentToWorld = mem::RPM<FTransform>(mesh + Engine::ComponentToWorld);

		FMatrix Matrix = Bone.ToMatrixWithScale() * ComponentToWorld.ToMatrixWithScale();
		return FVector{ Matrix.WPlane.X, Matrix.WPlane.Y, Matrix.WPlane.Z };
	}

	FVector Engine::WorldToScreen(FVector WorldLocation, FCameraCacheEntry CameraCacheL)
	{
		FVector Screenlocation{};

		auto POV = CameraCacheL.POV;
		FVector Rotation = POV.Rotation;

		FMatrix tempMatrix = ToMatrix(Rotation);

		FVector vAxisX, vAxisY, vAxisZ;

		vAxisX = FVector{ tempMatrix.XPlane.X, tempMatrix.XPlane.Y, tempMatrix.XPlane.Z };
		vAxisY = FVector{ tempMatrix.YPlane.X, tempMatrix.YPlane.Y, tempMatrix.YPlane.Z };
		vAxisZ = FVector{ tempMatrix.ZPlane.X, tempMatrix.ZPlane.Y, tempMatrix.ZPlane.Z };

		FVector vDelta = WorldLocation - POV.Location;
		FVector vTransformed = FVector{ vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX) };

		if (vTransformed.Z < 1.f)
			vTransformed.Z = 1.f;

		float FovAngle = POV.FOV;
		float ScreenCenterX = cRenderer::Width / 2;
		float ScreenCenterY = cRenderer::Height / 2;

		Screenlocation.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
		Screenlocation.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;

		return Screenlocation;
	}

	bool Engine::IsMale(uint32_t obj_id)
	{
		if (std::find(std::begin(MaleIds), std::end(MaleIds), obj_id) != std::end(MaleIds))
			return true;
		return false;
	}

	bool Engine::IsFemale(uint32_t obj_id)
	{
		if (std::find(std::begin(FemaleIds), std::end(FemaleIds), obj_id) != std::end(FemaleIds))
			return true;
		return false;
	}

	const char* Engine::GetNameFromID(int ID)
	{
		uint64_t namePoolChunk = mem::RPM<uint64_t>(Engine::GNames + (((uint32_t)(ID >> 16) + 2) * 8));
		uint64_t entryOffset = namePoolChunk + (uint32_t)(4 * (uint16_t)ID);
		uint16_t nameLength = min(mem::RPM<uint16_t>(entryOffset + 4) >> 1, 255);

		FText name = mem::RPM<FText>(entryOffset + 6);
		name.szChar[nameLength] = '\0';

		return name.szChar;
	}

}