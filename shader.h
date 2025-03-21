#pragma once
#include <WICTextureLoader.h> 
#include <d3d11.h>

//using namespace DirectX;
//#include <wrl.h>
//using namespace Microsoft::WRL;
//#include <string>
//#include <map>
using namespace std;

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, 
	ID3D11VertexShader** vertex_shader,ID3D11InputLayout** input_layout, 
	D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name,
	                       ID3D11PixelShader** pixel_shader);