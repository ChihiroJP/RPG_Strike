#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include <algorithm>
#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include "Map.h"

// Globals
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11Device* g_pDevice = nullptr;
ID3D11DeviceContext* g_pContext = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
Renderer* g_pRenderer = nullptr;
Player* g_pPlayer = nullptr;
Map* g_pMap = nullptr;
std::vector<Enemy> g_enemies;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void DrawUI(Renderer* renderer, Player* player)
{
    float p_hp = (float)player->GetHP() / player->GetMaxHP();
    renderer->DrawQuad(20, 1030, 400, 30, 0.2f, 0.0f, 0.0f, 1.0f);
    renderer->DrawQuad(20, 1030, 400 * p_hp, 30, 0.0f, 0.8f, 0.0f, 1.0f);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    HWND hwnd;
    {
        const wchar_t CLASS_NAME[] = L"RPG Game Window Class";
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW); wc.style = CS_HREDRAW | CS_VREDRAW; wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance; wc.lpszClassName = CLASS_NAME; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassExW(&wc);
        RECT wr = { 0, 0, 1920, 1080 };
        AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
        hwnd = CreateWindowExW(0, CLASS_NAME, L"2D RPG Game", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
    }

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1; scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; scd.BufferDesc.Width = 1920; scd.BufferDesc.Height = 1080;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; scd.OutputWindow = hwnd; scd.SampleDesc.Count = 1; scd.Windowed = TRUE;
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &scd, &g_pSwapChain, &g_pDevice, NULL, &g_pContext);

    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
    pBackBuffer->Release();

    D3D11_VIEWPORT vp;
    vp.Width = 1920.0f;
    vp.Height = 1080.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pContext->RSSetViewports(1, &vp);

    g_pRenderer = new Renderer();
    g_pRenderer->Init(g_pDevice, g_pContext);

    g_pMap = new Map();
    g_pMap->Load();

    g_pPlayer = new Player();
    g_pPlayer->Init(100.0f, 100.0f, 150.0f);

    Enemy enemy1; enemy1.Init(500, 500);
    g_enemies.push_back(enemy1);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    LARGE_INTEGER lastTime, currentTime, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
    float deltaTime = 0.0f;

    MSG msg = {};
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg); DispatchMessage(&msg);
        } else {
            QueryPerformanceCounter(&currentTime);
            deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / (float)frequency.QuadPart;
            lastTime = currentTime;

            g_pPlayer->Update(deltaTime, g_pMap);
            for(auto& enemy : g_enemies) enemy.Update(deltaTime);
            g_pPlayer->HandleCollisions(g_enemies);

            g_pPlayer->m_bullets.erase(std::remove_if(g_pPlayer->m_bullets.begin(), g_pPlayer->m_bullets.end(), [](const Bullet& b) { return !b.IsActive(); }), g_pPlayer->m_bullets.end());
            g_enemies.erase(std::remove_if(g_enemies.begin(), g_enemies.end(), [](const Enemy& e) { return !e.IsActive(); }), g_enemies.end());

            g_pRenderer->BeginFrame(g_pRenderTargetView);
            g_pMap->Draw(g_pRenderer);
            g_pPlayer->Draw(g_pRenderer);
            for(auto& enemy : g_enemies) enemy.Draw(g_pRenderer);
            DrawUI(g_pRenderer, g_pPlayer);
            g_pRenderer->EndFrame(g_pSwapChain);
        }
    }

    delete g_pRenderer; delete g_pPlayer; delete g_pMap;
    if (g_pSwapChain) g_pSwapChain->Release(); if (g_pDevice) g_pDevice->Release(); if (g_pContext) g_pContext->Release(); if (g_pRenderTargetView) g_pRenderTargetView->Release();

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}
