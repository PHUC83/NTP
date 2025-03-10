#include "plane.h"
#include "texture.h"
#include <assert.h>

Vertex planeVertices[] = {
    { { -10.0f, 0.0f, -10.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
    { {  10.0f, 0.0f, -10.0f }, { 0.0f, 1.0f, 0.0f }, { 5.0f, 0.0f } },
    { {  10.0f, 0.0f,  10.0f }, { 0.0f, 1.0f, 0.0f }, { 5.0f, 5.0f } },
    { { -10.0f, 0.0f,  10.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 5.0f } }
};

uint32_t planeIndices[] = {
    0, 1, 2,
    0, 2, 3
};

Plane::Plane(ID3D11Device* device)
    : device(device), vertexBuffer(nullptr), indexBuffer(nullptr),
    diffuseTextureView(nullptr), normalTextureView(nullptr)
{
}

Plane::~Plane()
{
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (diffuseTextureView) diffuseTextureView->Release();
    if (normalTextureView) normalTextureView->Release();
}

void Plane::Initialize()
{
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(Vertex) * planeVertexCount;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA vbData = { planeVertices, 0, 0 };
    HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);
    assert(SUCCEEDED(hr) && "Failed to create plane vertex buffer!");

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(uint32_t) * planeIndexCount;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA ibData = { planeIndices, 0, 0 };
    hr = device->CreateBuffer(&ibDesc, &ibData, &indexBuffer);
    assert(SUCCEEDED(hr) && "Failed to create plane index buffer!");

    // T?i texture diffuse v? normal
    D3D11_TEXTURE2D_DESC textureDesc;
    hr = load_texture_from_file(device, L".\\resources\\grass_diffuse.png", &diffuseTextureView, &textureDesc);
    assert(SUCCEEDED(hr) && "Failed to load grass_diffuse.png texture!");

    hr = load_texture_from_file(device, L".\\resources\\grass_normal.png", &normalTextureView, &textureDesc);
    assert(SUCCEEDED(hr) && "Failed to load grass_normal.png texture!");
}

void Plane::Render(ID3D11DeviceContext* context, ID3D11Buffer* objectConstantBuffer,
    ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader,
    ID3D11SamplerState* samplerState, XMFLOAT4X4 plane_world)
{
    struct ObjectConstants
    {
        XMFLOAT4X4 world;
        XMFLOAT4 material_color;
    } objectConstants;
    objectConstants.world = plane_world;
    objectConstants.material_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    context->UpdateSubresource(objectConstantBuffer, 0, nullptr, &objectConstants, 0, 0);

    // Bind vertex v? index buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Bind shader
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // Bind texture diffuse v? normal
    ID3D11ShaderResourceView* textureViews[] = { diffuseTextureView, normalTextureView };
    context->PSSetShaderResources(0, 1, textureViews); 
    context->PSSetSamplers(0, 1, &samplerState);

    // Draw
    context->DrawIndexed(planeIndexCount, 0, 0);
}