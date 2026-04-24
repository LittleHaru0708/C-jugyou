// CPP3-01.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "CPP3.h"

#include <directxmath.h>
#include <SpriteFont.h>
#include <SimpleMath.h>
#include "D3DManager.h"
#include "Shader.h"
#include "Input.h"

#define MAX_LOADSTRING 100


// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

using namespace UniDx;
using namespace DirectX;

// シェーダー
Shader spriteShader;

// フォント描画用
std::unique_ptr<SpriteBatch> g_spriteBatch;
std::unique_ptr<SpriteFont>  g_spriteFont;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CPP301, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CPP301));

    MSG msg;
    // メイン メッセージ ループ:
    while(true)
    {
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Windowsのメッセージ処理
            // 終了メッセージがきた
            if(msg.message == WM_QUIT) {
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        // 画面を塗りつぶす
        D3DManager::getInstance()->Clear(0.3f, 0.5f, 0.9f, 1.0f);

        // 描画処理
        // 三角形の描画
        {
            // １頂点の形式(今回は座標だけ)
            struct VertexType
            {
                DirectX::XMFLOAT3 Pos;	// 座標
            };
            // 三角形を作るため、頂点を３つ作る
            VertexType v[3] = {
                {{-0.5,-0.5,0}},
                {{-0.5,0.5,0}},
                {{0.5,-0.5,0}}
            };
            //-----------------------------
            // 頂点バッファ作成
            // ・ビデオメモリ側に「頂点バッファ」という形で作る必要がある
            // ・今回は描画のたびに作る
            //-----------------------------
            // 作成するバッファの仕様を決める
            D3D11_BUFFER_DESC vbDesc = {};
            vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
            vbDesc.ByteWidth = sizeof(v);					// 作成するバッファのバイトサイズ
            vbDesc.MiscFlags = 0;							// その他のフラグ
            vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
            vbDesc.Usage = D3D11_USAGE_DEFAULT;				// 作成するバッファの使用法
            vbDesc.CPUAccessFlags = 0;

            // 上の仕様を渡して頂点バッファを作ってもらう
            ComPtr<ID3D11Buffer> vb;
            D3D11_SUBRESOURCE_DATA initData = { &v[0], sizeof(v), 0 };	// 書き込むデータ

            // 頂点バッファの作成
            D3DManager::getInstance()->GetDevice()->CreateBuffer(&vbDesc, &initData, &vb);

            // 頂点バッファを描画で使えるようにセットする
            UINT stride = sizeof(VertexType);
            UINT offset = 0;
            D3DManager::getInstance()->GetContext()->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);

            // プロミティブ・トポロジーをセット
            D3DManager::getInstance()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

            //-----------------------------
            // シェーダーをセット
            //-----------------------------
            spriteShader.SetToContext();

            //-----------------------------
            // 描画実行
            //-----------------------------
            // デバイスコンテキスト、上記のセットした内容で描画する
            D3DManager::getInstance()->GetContext()->Draw(3, 0);
        }

        // バックバッファの内容を画面に表示
        D3DManager::getInstance()->Present();
    }


    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CPP301));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CPP301);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Direct3Dインスタンス作成g_constantBuffer
   D3DManager::create();

   // Direct3D初期化
   D3DManager::getInstance()->Initialize(hWnd, 1280, 720);

   // シェーダーコンパイル
   spriteShader.Compile(L"SpriteShader.hlsl");

   // フォント初期化
   g_spriteBatch = std::make_unique<SpriteBatch>(D3DManager::getInstance()->GetContext().Get());
   g_spriteFont = std::make_unique<SpriteFont>(D3DManager::getInstance()->GetDevice().Get(), L"M PLUS 1.spritefont");

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   Input::initialize();

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
