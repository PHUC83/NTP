#include "sprite.h" 
#include "misc.h" 
//#include<memory>
//#include <sstream> 
//#include <WICTextureLoader.h>
#include"texture.h"
#include"shader.h"



void sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy,      
	float dw, float dh ,   
	float r, float g, float b, float a,float angle,
	float sx, float sy, float sw, float sh)
	{

	//①スクリーン（ビューポート）のサイズを取得する 
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);	
	
	
	//②render メンバ関数の引数（dx, dy, dw, dh）から矩形の各頂点の位置（スクリーン座標系）を計算する 
	float x0{ dx };
	float y0{ dy };
	float u0{ sx };
	float v0{ sy };
	float x1{ dx + dw };
	float y1{ dy };
	float u1{ sx + sw };
	float v1{ sy };

	float x2{ dx };
	float y2{ dy + dh };
	float u2{ sx };
	float v2{ sy + sh };

	float x3{ dx + dw };
	float y3{ dy + dh };
	float u3{ sx + sw };
	float v3{ sy + sh };


	auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    {	x -= cx;
		y -= cy;
		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;
		x += cx;
		y += cy;
    };
	//回転の中心を矩形の中心点にした場合 
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);

	//③スクリーン座標系からNDCへの座標変換をおこなう
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	u0 /= texture2d_desc.Width;
	v0 /= texture2d_desc.Height;
	u1 /= texture2d_desc.Width;
	v1 /= texture2d_desc.Height;
	u2 /= texture2d_desc.Width;
	v2 /= texture2d_desc.Height;
	u3 /= texture2d_desc.Width;
	v3 /= texture2d_desc.Height;


	//④計算結果で頂点バッファオブジェクトを更新する 
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	vertex * vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (vertices != nullptr)
	{
	
	  vertices[0].position = { x0, y0 , 0 };
	  vertices[1].position = { x1, y1 , 0 };
	  vertices[2].position = { x2, y2 , 0 };
	  vertices[3].position = { x3, y3 , 0 };
	  vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };
	  
	  vertices[0].texcoord = { u0,v0 };
	  vertices[1].texcoord = { u1,v1 };
	  vertices[2].texcoord = { u2,v2 };
	  vertices[3].texcoord = { u3,v3 };
    }
	
	immediate_context->Unmap(vertex_buffer, 0);


	//①頂点バッファーのバインド 
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

    //②プリミティブタイプおよびデータの順序に関する情報のバインド
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//③入力レイアウトオブジェクトのバインド 
	immediate_context->IASetInputLayout(input_layout);

	//④シェーダーのバインド
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

		//②シェーダー リソースのバインド
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	
	//⑤プリミティブの描画 
	immediate_context->Draw(4, 0);




}

	void sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle)
	{

		render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle,0,0,texture2d_desc.Width,texture2d_desc.Height);
	}


//void sprite::render(ID3D11DeviceContext* immediate_context)
//{
//	//①頂点バッファーのバインド 
//	UINT stride{ sizeof(vertex) };
//	UINT offset{ 0 };
//	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
//
//	//②プリミティブタイプおよびデータの順序に関する情報のバインド
//	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//	//③入力レイアウトオブジェクトのバインド 
//	immediate_context->IASetInputLayout(input_layout);
//
//	//④シェーダーのバインド
//	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
//	immediate_context->PSSetShader(pixel_shader, nullptr, 0);
//
//	//⑤プリミティブの描画 
//	immediate_context->Draw(4, 0);  //draw(a,b)a is no. of point, b is from which no of data will it start 
//
//
//
//}

sprite::sprite(ID3D11Device* device, const wchar_t* filename)
{
	//①頂点情報のセット
	vertex vertices[]
	{
	  { { -0.5f, +0.5, 0 }, { 1, 1, 1, 1 },{0,0}},
	  { { +0.5f, +0.5, 0 }, { 1, 0, 0, 1 },{1,0}},
	  { { -0.5f, -0.5, 0 }, { 0, 1, 0, 1 },{0,1}},
	  { { +0.5f, -0.5, 0 }, { 0, 0, 1, 1 },{1,1}}
	};

	//②頂点バッファオブジェクトの生成 
	HRESULT hr(S_OK);

	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(vertices);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data{};
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc,
			&subresource_data, &vertex_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	//③頂点シェーダーオブジェクトの生成
	//const char* cso_name{ "sprite_vs.cso" };
	
	//FILE * fp{};
	//fopen_s(&fp, cso_name, "rb");
	//_ASSERT_EXPR_A(fp, "CSO File not found");
	//
	//fseek(fp, 0, SEEK_END);
	//long cso_sz{ ftell(fp) };
	//fseek(fp, 0, SEEK_SET);
	//
	//std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	//fread(cso_data.get(), cso_sz, 1, fp);
	//fclose(fp);
	//
	//hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		//④入力レイアウトオブジェクトの生成
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
		  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		  { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
              D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		//hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc),
		//	cso_data.get(), cso_sz, &input_layout);
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_vs_from_cso(device, "sprite_vs.cso",
			&vertex_shader, &input_layout, input_element_desc, _countof(input_element_desc));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	 
	////⑤ピクセルシェーダーオブジェクトの生成 
	//{
	// const char* cso_name{ "sprite_ps.cso" };

	// FILE* fp{};
	// fopen_s(&fp, cso_name, "rb");
	// _ASSERT_EXPR_A(fp, "CSO File not found");

	// fseek(fp, 0, SEEK_END);
	// long cso_sz{ ftell(fp) };
	// fseek(fp, 0, SEEK_SET);

	// std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	// fread(cso_data.get(), cso_sz, 1, fp);
	// fclose(fp);

	// HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader);
	// _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
		//const char* cso_name{ "sprite_ps.cso" };
		hr = create_ps_from_cso(device, "sprite_ps.cso", &pixel_shader);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	 ////③画像ファイルのロードとシェーダーリソースビューオブジェクト(ID3D11ShaderResourceView)の生成 
	 //ID3D11Resource * resource{};
	 //hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view);
	 //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	 ////④テクスチャ情報(D3D11_TEXTURE2D_DESC)の取得

	 //ID3D11Texture2D * texture2d{};
	 //hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
	 //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	 //texture2d->GetDesc(&texture2d_desc);
	 //texture2d->Release();	
	 //resource->Release();
	hr = load_texture_from_file(device, filename,
		&shader_resource_view, &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//release_all_textures();

}

sprite::~sprite()
{
	shader_resource_view->Release();
	vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();
}
