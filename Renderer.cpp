#include "Renderer.h"
#include <d3dcompiler.h>
#include <vector>

const char g_ShaderCode[] =
"cbuffer ConstantBuffer : register(b0)"
"{"
"    matrix projectionMatrix;"
"}"
"struct VS_INPUT { float2 pos : POSITION; float4 col : COLOR; };"
"struct PS_INPUT { float4 pos : SV_POSITION; float4 col : COLOR; };"
"PS_INPUT VS(VS_INPUT input)"
"{"
"    PS_INPUT output = (PS_INPUT)0;"
"    output.pos = mul(projectionMatrix, float4(input.pos.x, input.pos.y, 0.0f, 1.0f));"
"    output.col = input.col;"
"    return output;"
"}"
"float4 PS(PS_INPUT input) : SV_Target { return input.col; }";

Renderer::Renderer() {}
Renderer::~Renderer() {}

bool Renderer::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_device = device;
    m_context = context;

    ID3DBlob *vsBlob = nullptr, *psBlob = nullptr;
    D3DCompile(g_ShaderCode, strlen(g_ShaderCode), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &vsBlob, NULL);
    D3DCompile(g_ShaderCode, strlen(g_ShaderCode), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &psBlob, NULL);

    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &m_vertexShader);
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &m_pixelShader);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    m_device->CreateInputLayout(ied, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

    vsBlob->Release(); psBlob->Release();

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * MAX_QUADS * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_device->CreateBuffer(&bd, NULL, &m_vertexBuffer);

    std::vector<unsigned short> indices(MAX_QUADS * 6);
    for(int i = 0; i < MAX_QUADS; ++i) {
        indices[i*6+0] = i*4+0; indices[i*6+1] = i*4+1; indices[i*6+2] = i*4+2;
        indices[i*6+3] = i*4+2; indices[i*6+4] = i*4+3; indices[i*6+5] = i*4+0;
    }
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_IMMUTABLE; ibd.ByteWidth = sizeof(unsigned short) * MAX_QUADS * 6; ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA isd = {indices.data(), 0, 0};
    m_device->CreateBuffer(&ibd, &isd, &m_indexBuffer);

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT; cbd.ByteWidth = sizeof(ConstantBuffer); cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_device->CreateBuffer(&cbd, NULL, &m_constantBuffer);

    XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0.0f, 1920.0f, 1080.0f, 0.0f, 0.1f, 100.0f);
    m_context->UpdateSubresource(m_constantBuffer, 0, NULL, &projection, 0, 0);

    return true;
}

void Renderer::Shutdown()
{
    if (m_vertexShader) m_vertexShader->Release(); if (m_pixelShader) m_pixelShader->Release();
    if (m_inputLayout) m_inputLayout->Release(); if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release(); if (m_constantBuffer) m_constantBuffer->Release();
}

void Renderer::BeginFrame(ID3D11RenderTargetView* rtv)
{
    m_renderTargetView = rtv;
    float color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_context->ClearRenderTargetView(m_renderTargetView, color);
    m_quadCount = 0;
}

void Renderer::DrawQuad(float x, float y, float width, float height, float r, float g, float b, float a)
{
    if(m_quadCount >= MAX_QUADS) return;
    int i = m_quadCount * 4;
    m_vertices[i+0] = {x, y, r, g, b, a};
    m_vertices[i+1] = {x + width, y, r, g, b, a};
    m_vertices[i+2] = {x + width, y + height, r, g, b, a};
    m_vertices[i+3] = {x, y + height, r, g, b, a};
    m_quadCount++;
}

void Renderer::EndFrame(IDXGISwapChain* swapChain)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, m_vertices, sizeof(Vertex) * m_quadCount * 4);
    m_context->Unmap(m_vertexBuffer, 0);

    UINT stride = sizeof(Vertex); UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    m_context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_context->IASetInputLayout(m_inputLayout);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_context->VSSetShader(m_vertexShader, 0, 0);
    m_context->PSSetShader(m_pixelShader, 0, 0);
    m_context->VSSetConstantBuffers(0, 1, &m_constantBuffer);
    m_context->DrawIndexed(m_quadCount * 6, 0, 0);

    swapChain->Present(1, 0);
}
