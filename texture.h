#pragma once
#include <WICTextureLoader.h> 
using namespace DirectX;
#include <d3d11.h>
#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include <map>
using namespace std;

HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename,
    ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);
void release_all_textures();
HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension);

