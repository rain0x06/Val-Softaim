#include "function.hpp"

namespace Prestige
{

	void function::FunctionExecutor()
	{
		__try
		{
			Engine::Update();

			ESP();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}
	}

	void function::ESP()
	{
		if (Engine::ActorCounts < 50000)
		{
			FCameraCacheEntry CameraCache = mem::RPM<FCameraCacheEntry>(Engine::PlayerCameraManager + Engine::CameraCache);

			for (int i = 0; i < Engine::ActorCounts; i++)
			{
				uint64_t Actor = mem::RPM<uint64_t>(Engine::Actors + i * 8);

				if (!Actor || Actor == Engine::AcknowledgedPawn)
					continue;

				uint32_t obj_id = mem::RPM<uint32_t>(Actor + Engine::ObjectID);

				bool IsMale = Engine::IsMale(obj_id);
				bool IsFemale = Engine::IsFemale(obj_id);

				if (IsMale || IsFemale)
				{
					uint64_t DamageHandler = mem::RPM<uint64_t>(Actor + Engine::DamageHandler);
					FVector CachedLife = mem::RPM<FVector>(DamageHandler + Engine::CachedLife);
					if (CachedLife.X)
					{
						uint64_t PlayerState = mem::RPM<uint64_t>(Actor + Engine::PlayerState);
						uint64_t TeamComponent = mem::RPM<uint64_t>(PlayerState + Engine::TeamComponent);
						uint8_t Team = mem::RPM<uint8_t>(TeamComponent + Engine::Team);
						if (Team != Engine::LocalTeam)
						{
							uint64_t Mesh = mem::RPM<uint64_t>(Actor + Engine::Mesh);
							bool IsDormant = mem::RPM<bool>(Actor + Engine::IsDormant);

							FVector root = Engine::GetBoneWithRotation(Mesh, 0);
							FVector spos = Engine::WorldToScreen(root, CameraCache);
							FString PlayerName = mem::RPM<FString>(mem::RPM<uint64_t>(PlayerState + Engine::PlayerName));
							std::string PlayerNameStr = utils::utf8_encode(PlayerName.szChar);
							cRenderer::DrawOutlinedText(PlayerNameStr + "\n" + std::to_string((int)CachedLife.X), ImVec2(spos.X, spos.Y), 14, 0xFFFFFFFF, true);

							Color health{ (uint8_t)min((510 * (100 - (int)(CachedLife.X / CachedLife.Y * 100))) / 100, 255), (uint8_t)min((510 * (int)(CachedLife.X / CachedLife.Y * 100)) / 100, 255), 0, 255 };
							cRenderer::DrawSkeleton(Mesh, CameraCache, IsDormant ? 0xFFFFFFFF : health.RGBA2ARGB(255), IsMale);
						}
					}
				}
				else if (obj_id == Engine::SpyCamId || obj_id == Engine::ClayBombId || obj_id == Engine::SovaDroneId)
				{
					uint64_t Mesh = mem::RPM<uint64_t>(Actor + Engine::Mesh);

					FVector root = Engine::GetBoneWithRotation(Mesh, 0);
					FVector spos = Engine::WorldToScreen(root, CameraCache);
					cRenderer::DrawCircleFilled(ImVec2(spos.X, spos.Y), 2, 0xFFFF0000);
					if (obj_id == Engine::SpyCamId)
						cRenderer::DrawOutlinedText("Camera", ImVec2(spos.X, spos.Y), 14, 0xFFFF0000, true);
					else if (obj_id == Engine::ClayBombId)
						cRenderer::DrawOutlinedText("Boomba", ImVec2(spos.X, spos.Y), 14, 0xFFFF0000, true);
					else if (obj_id == Engine::SovaDroneId)
						cRenderer::DrawOutlinedText("Drone", ImVec2(spos.X, spos.Y), 14, 0xFFFF0000, true);
				}
			}
		}
	}

}