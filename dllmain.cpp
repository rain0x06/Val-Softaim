#pragma warning(disable:4996)

#include <Windows.h>
#include <thread>
#include <iostream>
#include <mutex>

#include "function.hpp"
#include "engine.hpp"
#include "d3d/renderer.hpp"
#include "d3d/hooker.hpp"

using namespace Prestige;

std::once_flag once;
void* o_present;
HRESULT hk_present(IDXGISwapChain* dis, UINT syncInterval, UINT flags)
{
	std::call_once(once, [dis]() {
		Engine::Init();
		cRenderer::Initialize(dis);
		});

	cRenderer::BeginScene();

	cRenderer::DrawOutlinedText("Prestige", ImVec2(10, 10), 16, 0xFFFFFFFF, false);

	function::FunctionExecutor();

	cRenderer::EndScene();

	return reinterpret_cast<decltype(&hk_present)>(o_present)(dis, syncInterval, flags);
}

void* o_resizebuffers;
HRESULT hk_resizebuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
{
	cRenderer::PreResize();
	auto hr = reinterpret_cast<decltype(&hk_resizebuffers)>(o_resizebuffers)(dis, bufferCount, width, height, newFormat, swapChainFlags);
	cRenderer::PostResize();

	return hr;
}

void MainThread()
{
	if (!AttachConsole(GetCurrentProcessId()))
		AllocConsole();

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);

	for (;;)
	{
		Sleep(1);

		/*if (GetAsyncKeyState(VK_F1))
		{
			std::cout << "dwGameBase: " << std::hex << mem::dwGameBase << std::dec << std::endl;
			std::cout << "UWorld: " << std::hex << Engine::UWorld << std::dec << std::endl;
			std::cout << "PersistentLevel: " << std::hex << Engine::PersistentLevel << std::dec << std::endl;
			std::cout << "OwningGameInstance: " << std::hex << Engine::OwningGameInstance << std::dec << std::endl;
			std::cout << "LocalPlayers: " << std::hex << Engine::LocalPlayers << std::dec << std::endl;
			std::cout << "LocalPlayer: " << std::hex << Engine::LocalPlayer << std::dec << std::endl;
			std::cout << "PlayerController: " << std::hex << Engine::PlayerController << std::dec << std::endl;
			std::cout << "PlayerCameraManager: " << std::hex << Engine::PlayerCameraManager << std::dec << std::endl;
			std::cout << "AcknowledgedPawn: " << std::hex << Engine::AcknowledgedPawn << std::dec << std::endl;
			std::cout << "Actors: " << std::hex << Engine::Actors << std::dec << std::endl;
			std::cout << "ActorCounts: " << std::hex << Engine::ActorCounts << std::dec << std::endl;
			Sleep(300);
		}*/
	}

	FreeConsole();
}

BOOL DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		//mem::WPM<WORD>(Engine::retaddr, 0x27FF);
		HookD3D11();
		//_beginthread((_beginthread_proc_type)MainThread, 0, nullptr);
	}

	return true;
}