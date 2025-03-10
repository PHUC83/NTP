#pragma once
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include<d3d11.h>
#include <wrl.h>

#include "sprite.h"
#include "sprite_batch.h"
#include"texture.h"


#include "misc.h"
#include "high_resolution_timer.h"
#include "geometric_primitive.h"
#include"static_mesh.h"
#include"skinned_mesh.h"

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "plane.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"X3DGP" };

class framework
{
public:
	CONST HWND hwnd;


	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;






	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device.Get(), immediate_context.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update(tictoc.time_interval());
				render(tictoc.time_interval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 1
		BOOL fullscreen = 0;
		swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			swap_chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();
	void update_camera();

private:
	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void calculate_frame_stats()
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}


private:
	//ID3D11Device* device;
	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> immediate_context = nullptr;
	Microsoft::WRL::ComPtr <IDXGISwapChain> swap_chain = nullptr;
	ID3D11RenderTargetView* render_target_view;
	ID3D11DepthStencilView* depth_stencil_view;
	ID3D11SamplerState* sampler_states[3];
	ID3D11DepthStencilState* depth_stencil_states[4];
	ID3D11BlendState* blend_states[5];
	
	DirectX::XMFLOAT3 player_position = { 0.0f, 0.5f, 0.0f }; // Dùng XMFLOAT3 thay cho float[3]
	float player_yaw = 0.0f;
	float player_pitch = 0.0f;
	DirectX::XMFLOAT3 camera_offset = { 0.0f, 0.0f, 0.0f }; // Dùng XMFLOAT3 cho offset
	float move_speed = 5.0f;

	float camera_yaw = 0.0f;
	float camera_pitch = 0.0f;
	float camera_distance = 10.0f;
	POINT last_mouse_pos = {};
	bool is_camera_rotating = false;
	//float dx, dy, dw, dh;
	struct scene_constants
	{
		DirectX::XMFLOAT4X4 view_projection;         //ビュー・プロジェクション変換行列 
		DirectX::XMFLOAT4 light_direction;
		DirectX::XMFLOAT4 camera_position;
		DirectX::XMFLOAT4 sunColor = { 1.0f, 0.95f, 0.8f, 1.0f }; // Màu vàng nhạt
    float sunIntensity = 1.5f;                                  // Độ mạnh
    DirectX::XMFLOAT3 padding;
		//ライトの向き 
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

	void update_scene_constants(const scene_constants& data);
	std::unique_ptr<skinned_mesh> skinned_meshes[8] = {};
	sprite* sprites[8] = {};
	sprite_batch* sprite_batches[8] = {};
	std::unique_ptr<geometric_primitive> geometric_primitives[8];
	std::unique_ptr<static_mesh> static_meshes[8] = {}; 
	std::unique_ptr<geometric_primitive> bouding_box;
		ComPtr<ID3D11RasterizerState> rasterizer_states[3];
	std::unique_ptr<skinned_mesh> player;
	Plane* plane;
};

