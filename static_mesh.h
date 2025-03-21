
#pragma once
#include <d3d11.h> 
#include <directxmath.h> 
#include <wrl.h>
#include "misc.h" 
#include "shader.h"
#include <vector>              
#include <DirectXMath.h> 
#include<fstream>
#include"texture.h"
#include"geometric_primitive.h"
class static_mesh
{
public:
struct vertex
{
DirectX::XMFLOAT3 position;
DirectX::XMFLOAT3 normal;
DirectX::XMFLOAT2 texcoord;
};
struct constants
{
DirectX::XMFLOAT4X4 world;
DirectX::XMFLOAT4 material_color;
};
struct subset
{
	std::wstring usemtl;
	uint32_t index_start{ 0 }; // start position of index buffer
	uint32_t index_count{ 0 }; // number of vertices (indices)
};
std::vector<subset> subsets;
 //std::wstring texture_filename;
//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	 struct material
	 {
	 std::wstring name;
	 DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
	 DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
	 DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };
	 std::wstring texture_filenames[2];
	 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];
	 //std::wstring texture_filename;
	 //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	 };
std::vector<material> materials;
DirectX::XMFLOAT3 maxV { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX };
DirectX::XMFLOAT3 minV { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX };

private:
Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
geometric_primitive *primivite=nullptr;
Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
//std::wstring texture_filename;
//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
std::wstring texture_filenames[2];
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];
		
public:
	void render_bounding_box(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& color);
static_mesh(ID3D11Device * device, const wchar_t* obj_filename);
virtual ~static_mesh() = default;	
void render(ID3D11DeviceContext * immediate_context,
const DirectX::XMFLOAT4X4 & world, const DirectX::XMFLOAT4 & material_color);
protected:
void create_com_buffers(ID3D11Device * device, vertex * vertices, size_t vertex_count,
uint32_t * indices, size_t index_count);
};