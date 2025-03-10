#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 texcoord;
};

extern Vertex planeVertices[];
extern uint32_t planeIndices[];

const UINT planeVertexCount = 4;
const UINT planeIndexCount = 6;

class Plane
{
public:
    Plane(ID3D11Device* device);
    ~Plane();
    void Initialize();
    void Render(ID3D11DeviceContext* context, ID3D11Buffer* objectConstantBuffer, 
                ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader,
                ID3D11SamplerState* samplerState, XMFLOAT4X4 plane_world);

private:
    ID3D11Device* device;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11ShaderResourceView* diffuseTextureView;
    ID3D11ShaderResourceView* normalTextureView;
};