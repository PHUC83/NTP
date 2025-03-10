#pragma once
#include <d3d11.h> 
#include <directxmath.h> 
#include<vector>

class sprite_batch
{
private:
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	ID3D11InputLayout* input_layout;
	ID3D11Buffer* vertex_buffer;
	ID3D11ShaderResourceView* shader_resource_view;
	//ID3D11ShaderResourceView** shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	//D3D11_TEXTURE2D_DESC* texture2d_desc;
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
	const size_t max_vertices;
	std::vector<vertex> vertices;


public:	
	sprite_batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
	~sprite_batch();

	void begin(ID3D11DeviceContext* immediate_context);
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle, float sx, float sy, float sw, float sh);
	//void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh,
	//	float r, float g, float b, float a, float angle);
	void end(ID3D11DeviceContext* immediate_context);
	//void render(ID3D11DeviceContext* immediate_context);


};