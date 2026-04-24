// CPP3-01.cpp : アプリケーションのエントリ ポイントを定義します。

#include "framework.h"
#include "CPP3.h"

#include <directxmath.h>
#include <SpriteFont.h>
#include <SimpleMath.h>
#include "D3DManager.h"
#include "Shader.h"
#include "Input.h"

#define MAX_LOADSTRING 100

// =========================
// グローバル変数
// =========================
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

using namespace UniDx;
using namespace DirectX;

// シェーダー
Shader spriteShader;

// フォント
std::unique_ptr<SpriteBatch> g_spriteBatch;
std::unique_ptr<SpriteFont>  g_spriteFont;

// =========================
// 頂点データ（グローバル）
// =========================
struct VertexType
{
    DirectX::XMFLOAT3 Pos;
};

VertexType v[3] =
{
    {{-0.5f,-0.5f,0}},
    {{-0.5f, 0.5f,0}},
    {{ 0.5f,-0.5f,0}}
};

// 頂点バッファ（1回生成）
ComPtr<ID3D11Buffer> g_vertexBuffer;

// 関数宣言
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// =========================
// メイン
// =========================
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CPP301, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    MSG msg = {};

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // =========================
        // 入力更新
        // =========================
        Input::update();

        float speed = 0.01f;

        if (Input::GetKey(Keyboard::A))
            for (int i = 0; i < 3; i++) v[i].Pos.x -= speed;

        if (Input::GetKey(Keyboard::D))
            for (int i = 0; i < 3; i++) v[i].Pos.x += speed;

        if (Input::GetKey(Keyboard::W))
            for (int i = 0; i < 3; i++) v[i].Pos.y += speed;

        if (Input::GetKey(Keyboard::S))
            for (int i = 0; i < 3; i++) v[i].Pos.y -= speed;

        // =========================
        // 画面クリア
        // =========================
        D3DManager::getInstance()->Clear(0.3f, 0.5f, 0.9f, 1.0f);

        // =========================
        // 描画
        // =========================
        {
            UINT stride = sizeof(VertexType);
            UINT offset = 0;

            D3DManager::getInstance()->GetContext()->IASetVertexBuffers(
                0, 1, g_vertexBuffer.GetAddressOf(), &stride, &offset);

            D3DManager::getInstance()->GetContext()->IASetPrimitiveTopology(
                D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

            spriteShader.SetToContext();

            D3DManager::getInstance()->GetContext()->Draw(3, 0);
        }

        D3DManager::getInstance()->Present();
    }

    return (int)msg.wParam;
}

// =========================
// ウィンドウ登録
// =========================
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPP301));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = szWindowClass;

    return RegisterClassExW(&wcex);
}

// =========================
// 初期化
// =========================
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        1280, 720,
        nullptr, nullptr,
        hInstance,
        nullptr);

    if (!hWnd)
        return FALSE;

    // DirectX初期化
    D3DManager::create();
    D3DManager::getInstance()->Initialize(hWnd, 1280, 720);

    // シェーダー
    spriteShader.Compile(L"SpriteShader.hlsl");

    // フォント
    g_spriteBatch = std::make_unique<SpriteBatch>(
        D3DManager::getInstance()->GetContext().Get());

    g_spriteFont = std::make_unique<SpriteFont>(
        D3DManager::getInstance()->GetDevice().Get(),
        L"M PLUS 1.spritefont");

    // =========================
    // 頂点バッファ作成（1回だけ）
    // =========================
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.ByteWidth = sizeof(v);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = v;

    D3DManager::getInstance()->GetDevice()->CreateBuffer(
        &vbDesc,
        &initData,
        &g_vertexBuffer
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    Input::initialize();

    return TRUE;
}

// =========================
// ウィンドウ処理
// =========================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// About（未使用）
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM)
{
    return (INT_PTR)FALSE;
}