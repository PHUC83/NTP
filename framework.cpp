#include "framework.h"
#include <algorithm>
#include <cmath>
float yaw = 0.0f;   
float pitch = 0.0f;
//float curposmouseX;
//float curposmouseY;
//float deltaX = 0.0f;
//float deltaY = 0.0f;
float pos[2];
float size3[2];
float color1[4];
float rotationX;
float rotationY;
float rotationZ;


enum stencil_states
{
	BothOn,
	BothOff,
	TestOnWriteOff,
	TestOffWriteOn
};

framework::framework(HWND hwnd) : hwnd(hwnd)
{
}

bool framework::initialize()
{
	//①デバイス・デバイスコンテキスト・スワップチェーンの作成 
	HRESULT hr(S_OK);

	UINT create_device_flags{ 0 };
#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
	swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = !FULLSCREEN;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
		&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		swap_chain.GetAddressOf(), device.GetAddressOf(), NULL, immediate_context.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//レンダーターゲットビューの作成
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<LPVOID*>(&back_buffer));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device.Get()-> CreateRenderTargetView(back_buffer, NULL, &render_target_view);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	back_buffer->Release();


	//深度ステンシルビューの作成
	ID3D11Texture2D * depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = SCREEN_WIDTH;
	texture2d_desc.Height = SCREEN_HEIGHT;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
    texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
    texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture2d_desc.CPUAccessFlags = 0;
    texture2d_desc.MiscFlags = 0;
    hr = device.Get()->CreateTexture2D(&texture2d_desc, NULL,
		&depth_stencil_buffer);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
   
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format = texture2d_desc.Format;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
    hr = device.Get()->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
   
    depth_stencil_buffer->Release();

	//ビューポートの設定
    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(SCREEN_WIDTH);
    viewport.Height = static_cast<float>(SCREEN_HEIGHT);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    immediate_context->RSSetViewports(1, &viewport);


	{
		//② framework クラスのinitialize メンバ関数でサンプラーステートオブジェクトを生成する
		D3D11_SAMPLER_DESC sampler_desc;
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device.Get()->CreateSamplerState(&sampler_desc, &sampler_states[0]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = device.Get()->CreateSamplerState(&sampler_desc, &sampler_states[1]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device.Get()->CreateSamplerState(&sampler_desc, &sampler_states[2]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
	//②framework クラスのinitialize メンバ関数で深度ステンシルステートオブジェクトを作成する
		{
			D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device.Get()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[BothOn]);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			hr = device.Get()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[BothOff]);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));




			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			hr = device.Get()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[TestOnWriteOff]);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			hr = device.Get()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[TestOffWriteOn]);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
	{
		D3D11_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device.Get()->CreateBlendState(&blend_desc, &blend_states[0]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//無効
		blend_desc.RenderTarget[0].BlendEnable = FALSE;
		hr = device.Get()->CreateBlendState(&blend_desc, &blend_states[1]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//加算
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device.Get()->CreateBlendState(&blend_desc, &blend_states[2]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//減算
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device.Get()->CreateBlendState(&blend_desc, &blend_states[3]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//乗算
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device.Get()->CreateBlendState(&blend_desc, &blend_states[4]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	{
		//シーン定数バッファオブジェクトを生成する
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(scene_constants);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = TRUE;
		/*rasterizer_desc.FrontCounterClockwise = FALSE;*/
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;

		rasterizer_desc.AntialiasedLineEnable = TRUE;
	
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[2].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	/*sprites[0] = new sprite(device.Get(), L"./resources/cyberpunk.jpg");
	sprites[1] = new sprite(device.Get(), L"./resources/player.png");
	sprite_batches[0] = new sprite_batch(device.Get(), L"./resources/player.png", 2048);*/
	//geometric_primitives[0] = std::make_unique<cube>(device.Get());
	/*geometric_primitives[1] = std::make_unique<cylinder>(device.Get());*/
	//geometric_primitives[2] = std::make_unique<sphere>(device.Get());
	/*static_meshes[1] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\cube.obj");*/
	//static_meshes[1] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Cup\\cup.obj");
	//static_meshes[1] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Rock\\Rock.obj");
	//skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.000.fbx");
	skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.004.fbx", true);
	//skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\Jummo\\Jummo.fbx", true);
	//skinned_meshes[0]->load("path/to/model.fbx");
	//skinned_meshes[0]->load_animation("resources/Jummo/Animations/Idle.fbx", "Idle");
	//skinned_meshes[0]->load_animation("resources/Jummo/Animations/Walking.fbx", "Walking");
	//skinned_meshes[0]->load_animation("resources/Jummo/Animations/Running.fbx", "Running");
	//skinned_meshes[0]->play_animation("Idle");
	player = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\cube.004.fbx", true);
	plane = new Plane(device.Get());
	plane->Initialize();
	pos[0] = 0;
	pos[1] = 0;
	size3[0] = 1280;
	size3[1] = 720;
	color1[0] = 1;
	color1[1] = 0;
	color1[2] = 1;
	color1[3] = 1;
	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;
	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	float yaw_rad = XMConvertToRadians(player_yaw);
	XMVECTOR forward = XMVectorSet(std::sin(yaw_rad), 0, std::cos(yaw_rad), 0);
	XMVECTOR right = XMVectorSet(std::cos(yaw_rad), 0, -std::sin(yaw_rad), 0);
	float speed = move_speed * elapsed_time;

	if (GetAsyncKeyState('W') & 0x8000) // Tiến
	{
		XMVECTOR pos = XMLoadFloat3(&player_position);
		pos = pos + forward * speed;
		XMStoreFloat3(&player_position, pos);
	}
	if (GetAsyncKeyState('S') & 0x8000) // Lùi
	{
		XMVECTOR pos = XMLoadFloat3(&player_position);
		pos = pos - forward * speed;
		XMStoreFloat3(&player_position, pos);
	}
	if (GetAsyncKeyState('A') & 0x8000) // Trái
	{
		XMVECTOR pos = XMLoadFloat3(&player_position);
		pos = pos - right * speed;
		XMStoreFloat3(&player_position, pos);
	}
	if (GetAsyncKeyState('D') & 0x8000) // Phải
	{
		XMVECTOR pos = XMLoadFloat3(&player_position);
		pos = pos + right * speed;
		XMStoreFloat3(&player_position, pos);
	}

#ifdef USE_IMGUI
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
	//if (ImGui::Begin("ImGUI"));
	if (ImGui::Begin("ImGUI", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ImGui::InputFloat2("Position", pos);
			ImGui::SliderFloat("PositionX", &pos[0], 0, SCREEN_WIDTH);
			ImGui::SliderFloat("PositionY", &pos[1], 0, SCREEN_HEIGHT);
			ImGui::DragFloat("SizeX", &size3[0]);
			ImGui::DragFloat("SizeY", &size3[1]);
			//ImGui::InputFloat2("Size", size);
			ImGui::SliderFloat("Rotation X", &rotationX, -DirectX::XM_PI, DirectX::XM_PI);
			ImGui::SliderFloat("Rotation Y", &rotationY, -DirectX::XM_PI, DirectX::XM_PI);
			ImGui::SliderFloat("Rotation Z", &rotationZ, -DirectX::XM_PI, DirectX::XM_PI);
		}

		ImGui::ColorEdit4("Color", color1);
	}
	

	ImGui::End();
#endif
	update_camera();
}
void framework::update_scene_constants(const scene_constants& data)
{
	// Cập nhật constant buffer cho scene
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	HRESULT hr = immediate_context->Map(constant_buffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped_subresource.pData, &data, sizeof(scene_constants));
		immediate_context->Unmap(constant_buffers[0].Get(), 0);
	}
}
void framework::update_camera()
{
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		POINT current_mouse_pos;
		GetCursorPos(&current_mouse_pos); // Sửa lỗi cú pháp

		if (!is_camera_rotating)
		{
			last_mouse_pos = current_mouse_pos;
			is_camera_rotating = true;
		}

		float deltaX = static_cast<float>(current_mouse_pos.x - last_mouse_pos.x);
		float deltaY = static_cast<float>(current_mouse_pos.y - last_mouse_pos.y);

		const float mouse_sensitivity = 0.2f;
		player_yaw += deltaX * mouse_sensitivity;
		player_pitch += deltaY * mouse_sensitivity;
		player_pitch = (std::max)(-89.9f, (std::min)(player_pitch, 89.9f));

		last_mouse_pos = current_mouse_pos;
	}
	else
	{
		is_camera_rotating = false;
	}

	// Phóng to/thu nhỏ camera
	float mouse_wheel = 0.0f;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		mouse_wheel = GET_WHEEL_DELTA_WPARAM(0) / 120.0f;
		camera_distance -= mouse_wheel;
		camera_distance = (std::max)(1.0f, (std::min)(camera_distance, 100.0f));
	}
	
}

void framework::render(float elapsed_time)
{
	HRESULT hr{ S_OK };

	FLOAT color[]{ 0, 0, 0.2f, 1.0f };
	immediate_context->ClearRenderTargetView(render_target_view, color);
	immediate_context->ClearDepthStencilView(depth_stencil_view,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	immediate_context->PSSetSamplers(0, 1, &sampler_states[0]);
	immediate_context->PSSetSamplers(1, 1, &sampler_states[1]);
	immediate_context->PSSetSamplers(2, 1, &sampler_states[2]);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[BothOff], 1);
	immediate_context->OMSetBlendState(blend_states[0], nullptr, 0xFFFFFFFF);
	immediate_context->RSSetState(rasterizer_states[0].Get());
	const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // RHS Y-UP
{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // LHS Y-UP
{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // RHS Z-UP
{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // LHS Z-UP
	};
	// Tính góc và vị trí camera từ player
	float pitch_rad = DirectX::XMConvertToRadians(player_pitch);
	float yaw_rad = DirectX::XMConvertToRadians(player_yaw);
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(player_position.x, player_position.y, player_position.z, 1.0f); // Camera ở vị trí player
	DirectX::XMVECTOR forward = DirectX::XMVectorSet(std::sin(yaw_rad) * std::cos(pitch_rad), std::sin(pitch_rad), std::cos(yaw_rad) * std::cos(pitch_rad), 0);
	DirectX::XMVECTOR focus = eye + forward; // Điểm camera nhìn vào
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Trục lên chuẩn

	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);
	float aspect_ratio = viewport.Width / viewport.Height;

	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(eye, focus, up);
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), aspect_ratio, 0.1f, 100.0f); // FOV 60 độ

	// Ánh sáng chiếu ngược hướng nhìn của camera
	//DirectX::XMVECTOR light_dir = DirectX::XMVector3Normalize(-forward);
	DirectX::XMVECTOR light_dir = {0,-1,0,1.0f};

	scene_constants data{};
	DirectX::XMStoreFloat4(&data.light_direction, light_dir);
	DirectX::XMStoreFloat4x4(&data.view_projection, V * P);
	immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	// Debug ánh sáng
	DirectX::XMFLOAT4 light_dir_f4;
	DirectX::XMStoreFloat4(&light_dir_f4, light_dir);
	char debug_str[100];
	sprintf_s(debug_str, "Light Dir: %.2f, %.2f, %.2f\n", light_dir_f4.x, light_dir_f4.y, light_dir_f4.z);
	OutputDebugStringA(debug_str);

	// Render player (dùng player_position trực tiếp)
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&coordinate_system_transforms[3]);
	DirectX::XMMATRIX S_player = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	DirectX::XMMATRIX R_player = DirectX::XMMatrixRotationY(yaw_rad); // Player xoay theo yaw
	DirectX::XMMATRIX T_player = DirectX::XMMatrixTranslation(player_position.x, player_position.y, player_position.z);
	DirectX::XMFLOAT4X4 player_world;
	DirectX::XMStoreFloat4x4(&player_world, C * S_player * R_player * T_player);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[BothOn], 1);
	immediate_context->RSSetState(rasterizer_states[0].Get());
	player->render(immediate_context.Get(), player_world, { 1.0f, 0.0f, 0.0f, 1.0f }); // Màu đỏ


	{
		DirectX::XMMATRIX S_mesh = DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f);
		DirectX::XMMATRIX R_mesh = DirectX::XMMatrixRotationRollPitchYaw(rotationX, rotationY, rotationZ);
		DirectX::XMMATRIX T_mesh = DirectX::XMMatrixTranslation(0, 0, 0);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, C * S_mesh * R_mesh * T_mesh);
		immediate_context->OMSetDepthStencilState(depth_stencil_states[BothOn], 1);
		immediate_context->RSSetState(rasterizer_states[0].Get());
		skinned_meshes[0]->render(immediate_context.Get(), world, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	{
		XMMATRIX C = XMLoadFloat4x4(&coordinate_system_transforms[3]);
		XMMATRIX T_plane = XMMatrixTranslation(0.0f, 0.0f, 0.0f); // Đặt plane tại gốc
		XMFLOAT4X4 plane_world;
		XMStoreFloat4x4(&plane_world, C * T_plane);
		immediate_context->OMSetDepthStencilState(depth_stencil_states[BothOn], 1);
		plane->Render(immediate_context.Get(), constant_buffers[0].Get(), nullptr, nullptr, sampler_states[2],plane_world);
	}

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT sync_interval{ 0 };
	swap_chain.Get()->Present(sync_interval, 0);
}

bool framework::uninitialize()
{
	release_all_textures();
	for (ID3D11BlendState* p : blend_states)
	{
		if (p)
		{
			p->Release();
			p = nullptr;
		}

	}

	for (ID3D11DepthStencilState* p : depth_stencil_states)
	{
		if (p)
		{
			p->Release();
			p = nullptr;
		}

	}

	for (ID3D11SamplerState* p : sampler_states)
	{
		if (p)
		{
			p->Release();
			p = nullptr;
		}

	}

	for (sprite* p : sprites)
	{
		if (p)
		{
			delete p;
			p = nullptr;

		}
	}
	for (sprite_batch* p : sprite_batches)
	{
		if (p)
		{
			delete p;
			p = nullptr;

		}
	}

    //swap_chain->Release();
    depth_stencil_view->Release();
    render_target_view->Release();
    //immediate_context->Release();
    //device->Release();

	return true;
}

framework::~framework()
{

}