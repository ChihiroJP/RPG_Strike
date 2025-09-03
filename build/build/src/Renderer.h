#pragma once
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool Init(ID3D11Device* device, ID3D11DeviceContext* context);
    void Shutdown();

    void BeginFrame(ID3D11RenderTargetView* rtv);
    void DrawQuad(float x, float y, float width, float height, float r, float g, float b, float a);
    void EndFrame(IDXGISwapChain* swapChain);

private:
    struct Vertex { float x, y; float r, g, b, a; };
    struct ConstantBuffer { XMMATRIX projection; };

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_indexBuffer = nullptr;
    ID3D11Buffer* m_constantBuffer = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;

    static const int MAX_QUADS = 1000;
    Vertex m_vertices[MAX_QUADS * 4];
    int m_quadCount = 0;
};
