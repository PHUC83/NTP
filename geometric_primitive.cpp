#include "geometric_primitive.h"
#include "misc.h" 
#include "shader.h"
       
using namespace DirectX;
#ifndef XM_2PI
#define XM_2PI 6.28318530717958647692f
#endif
#ifndef XM_PI
#define XM_PI 3.14159265358979323846f
#endif
geometric_primitive::geometric_primitive(ID3D11Device* device)
{
   
    HRESULT hr{ S_OK };
   
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
       { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
             D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
       { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
             D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, "geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
           input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "geometric_primitive_ps.cso", pixel_shader.GetAddressOf());
   
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void geometric_primitive::create_com_buffers(ID3D11Device* device, vertex* vertices, 
	size_t vertex_count, uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };
	D3D11_BUFFER_DESC buffer_desc{};
    D3D11_SUBRESOURCE_DATA subresource_data{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    subresource_data.pSysMem = vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void geometric_primitive::render(ID3D11DeviceContext* immediate_context, 
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());
   
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
   
    // Calculate world inverse transpose
    DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&world);
    //DirectX::XMMATRIX WIT = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, W));
    //DirectX::XMFLOAT4X4 world_inverse_transpose;
    //DirectX::XMStoreFloat4x4(&world_inverse_transpose, WIT);

    //constants data{ world, world_inverse_transpose, material_color };
    constants data{ world, material_color };
    immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
   
    D3D11_BUFFER_DESC buffer_desc{};
    index_buffer->GetDesc(&buffer_desc);
    immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

cube::cube(ID3D11Device* device): geometric_primitive(device)
{
    // create a mesh for a cube
    vertex vertices[24]{};
    uint32_t indices[36]{};

    uint32_t face{ 0 };

    // top-side
    // 0---------1
    // |         |
    // |   -Y    |
    // |         |
    // 2---------3
    face = 0;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // bottom-side
    // 0---------1
    // |         |
    // |   -Y    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, -1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // front-side
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, -1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // back-side
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, +1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // right-side
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // left-side
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { -1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    create_com_buffers(device, vertices, 24, indices, 36);
}
cylinder::cylinder(ID3D11Device* device) : geometric_primitive(device)
{
    uint32_t slices=32;
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;

    float d{ 2.0f * XM_PI / slices };
    float r{ 0.5f };

    vertex vertex{};
    uint32_t base_index{ 0 };
   // top cap centre
    vertex.position = { 0.0f, +0.5f, 0.0f };
    vertex.normal = { 0.0f, +1.0f, 0.0f };
    vertices.emplace_back(vertex);
    // top cap ring
    for (uint32_t i = 0; i < slices; ++i)
    {
        float x{ r * cosf(i * d) };
        float z{ r * sinf(i * d) };
        vertex.position = { x, +0.5f, z };
        vertex.normal = { 0.0f, +1.0f, 0.0f };
        vertices.emplace_back(vertex);
    }
    base_index = 0;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        indices.emplace_back(base_index + 0);
        indices.emplace_back(base_index + i + 2);
        indices.emplace_back(base_index + i + 1);
    }
    indices.emplace_back(base_index + 0);
    indices.emplace_back(base_index + 1);
    indices.emplace_back(base_index + slices);

    // bottom cap centre
    vertex.position = { 0.0f, -0.5f, 0.0f };
    vertex.normal = { 0.0f, -1.0f, 0.0f };
    vertices.emplace_back(vertex);
    // bottom cap ring
    for (uint32_t i = 0; i < slices; ++i)
    {
        float x = r * cosf(i * d);
        float z = r * sinf(i * d);
        vertex.position = { x, -0.5f, z };
        vertex.normal = { 0.0f, -1.0f, 0.0f };
        vertices.emplace_back(vertex);
    }
    base_index = slices + 1;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        indices.emplace_back(base_index + 0);
        indices.emplace_back(base_index + i + 1);
        indices.emplace_back(base_index + i + 2);
    }
    indices.emplace_back(base_index + 0);
    indices.emplace_back(base_index + (slices - 1) + 1);
    indices.emplace_back(base_index + (0) + 1);

    // side rectangle
    for (uint32_t i = 0; i < slices; ++i)
    {
        float x = r * cosf(i * d);
        float z = r * sinf(i * d);

        vertex.position = { x, +0.5f, z };
        vertex.normal = { x, 0.0f, z };
        vertices.emplace_back(vertex);

        vertex.position = { x, -0.5f, z };
        vertex.normal = { x, 0.0f, z };
        vertices.emplace_back(vertex);
    }
    base_index = slices * 2 + 2;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        indices.emplace_back(base_index + i * 2 + 0);
        indices.emplace_back(base_index + i * 2 + 2);
        indices.emplace_back(base_index + i * 2 + 1);

        indices.emplace_back(base_index + i * 2 + 1);
        indices.emplace_back(base_index + i * 2 + 2);
        indices.emplace_back(base_index + i * 2 + 3);
    }
    indices.emplace_back(base_index + (slices - 1) * 2 + 0);
    indices.emplace_back(base_index + (0) * 2 + 0);
    indices.emplace_back(base_index + (slices - 1) * 2 + 1);

    indices.emplace_back(base_index + (slices - 1) * 2 + 1);
    indices.emplace_back(base_index + (0) * 2 + 0);
    indices.emplace_back(base_index + (0) * 2 + 1);

    create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

sphere::sphere(ID3D11Device* device)
    : geometric_primitive(device)
{
    // Parameters for the sphere
    const uint32_t segmentCount = 32;  // Number of segments (latitude)
    const uint32_t ringCount = 32;     // Number of rings (longitude)
    const float radius = 0.5f;         // Radius of the sphere

    const float angleStepLat = XM_PI / ringCount; // Angle between each latitude line
    const float angleStepLon = XM_2PI / segmentCount; // Angle between each longitude line

    // Allocate arrays for vertices and indices
    vertex vertices[2 + (segmentCount * ringCount)];  // 2 poles + vertices for each latitude and longitude
    uint32_t indices[(segmentCount * ringCount * 6)];  // 6 indices per triangle, 2 triangles per segment

    // Top and Bottom poles
    vertices[0].position = { 0.0f, 0.0f, radius };  // Top pole
    vertices[1].position = { 0.0f, 0.0f, -radius }; // Bottom pole
    vertices[0].normal = { 0.0f, 0.0f, 1.0f };      // Top pole normal
    vertices[1].normal = { 0.0f, 0.0f, -1.0f };     // Bottom pole normal

    // Generate vertices for the middle section
    uint32_t vertexIndex = 2;
    for (uint32_t i = 1; i < ringCount; ++i)
    {
        float theta = i * angleStepLat;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (uint32_t j = 0; j < segmentCount; ++j)
        {
            float phi = j * angleStepLon;
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            // Generate vertex positions
            vertices[vertexIndex].position = { radius * sinTheta * cosPhi, radius * sinTheta * sinPhi, radius * cosTheta };
            vertices[vertexIndex].normal = { sinTheta * cosPhi, sinTheta * sinPhi, cosTheta };
            ++vertexIndex;
        }
    }

    // Create indices for the top and bottom faces
    uint32_t index = 0;
    for (uint32_t i = 0; i < segmentCount; ++i)
    {
        indices[index++] = 0; // Top pole center
        indices[index++] = 2 + (i + 1) % segmentCount; // Next vertex in the top ring
        indices[index++] = 2 + i; // Current vertex in the top ring
    }

    for (uint32_t i = 0; i < segmentCount; ++i)
    {
        indices[index++] = 1; // Bottom pole center
        indices[index++] = 2 + (ringCount - 2) * segmentCount + i; // Next vertex in the bottom ring
        indices[index++] = 2 + (ringCount - 2) * segmentCount + (i + 1) % segmentCount; // Current vertex in the bottom ring
    }

    // Create indices for the side surface (side faces)
    for (uint32_t i = 0; i < ringCount - 2; ++i)
    {
        for (uint32_t j = 0; j < segmentCount; ++j)
        {
            uint32_t topIndex = 2 + i * segmentCount + j;
            uint32_t bottomIndex = 2 + (i + 1) * segmentCount + j;

            uint32_t nextTopIndex = 2 + i * segmentCount + ((j + 1) % segmentCount);
            uint32_t nextBottomIndex = 2 + (i + 1) * segmentCount + ((j + 1) % segmentCount);

            // First triangle of the side face
            indices[index++] = topIndex;
            indices[index++] = nextBottomIndex;
            indices[index++] = bottomIndex;

            // Second triangle of the side face
            indices[index++] = topIndex;
            indices[index++] = nextTopIndex;
            indices[index++] = nextBottomIndex;
        }
    }

    // Create buffers using the vertices and indices
    create_com_buffers(device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices));
}
