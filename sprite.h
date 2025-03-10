#pragma once
#include <d3d11.h> 
#include <directxmath.h> 
#include <wrl.h>

class sprite
{
private:
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader*  pixel_shader;
	ID3D11InputLayout*  input_layout;
	ID3D11Buffer*       vertex_buffer;
	ID3D11ShaderResourceView* shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

public:
	void render(ID3D11DeviceContext* immediate_context,float dx,float dy,float dw,float dh,
		float r, float g, float b, float a,
		float angle, float sx, float sy, float sw, float sh);
	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh,
		float r, float g, float b, float a, float angle);

	//void render(ID3D11DeviceContext* immediate_context);

	sprite(ID3D11Device* device, const wchar_t* filename);
	~sprite();
 
};