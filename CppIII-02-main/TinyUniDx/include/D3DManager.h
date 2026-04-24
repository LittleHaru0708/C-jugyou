/**
 * @file D3DManager.h
 * @brief DirectXの3D描画機能を提供する
 */
#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <memory>
#include <assert.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

constexpr UINT UNIDX_PS_SLOT_LIGHTS = 0;  // t0
constexpr UINT UNIDX_PS_SLOT_LIGHT_INDICES = 1;  // t1
constexpr UINT UNIDX_PS_SLOT_ALBEDO = 4;  // t4
constexpr UINT UNIDX_PS_SLOT_NORMAL = 5;  // t5


namespace UniDx{


/// @brief DirectXの3D描画機能を提供する
class D3DManager
{
public:
	// インスタンスの取得
	static D3DManager* getInstance() { return instance_.get(); }

	// このクラスをインスタンスとして作成
	static void create()
	{
		assert(instance_ == nullptr);
		instance_ = std::make_unique<D3DManager>();
	}

	// インスタンスの破棄
	static void destroy()
	{
		instance_.reset();
	}

	/**
	 * @brief Direct3Dを初期化し、使用できるようにする
	 * @param hWnd ウィンドウハンドル
	 * @param width 画面の幅
	 * @param height 画面の高さ
	 */
	bool Initialize(HWND hWnd, int width, int height);

	const ComPtr<ID3D11Device>&			GetDevice() const { return m_device; }
	const ComPtr<ID3D11DeviceContext>&	GetContext() const { return m_context; }

	// デストラクタ
	~D3DManager();

	// バックバッファレンダーターゲットをクリア
	void Clear(float r, float g, float b, float a);

	// バックバッファの内容を画面に表示
	void Present()
	{
		m_swapChain->Present(1, 0);
	}


private:
	static std::unique_ptr<D3DManager> instance_;

	ComPtr<ID3D11Device>			m_device; // Direct3Dデバイス
	ComPtr<ID3D11DeviceContext>		m_context; // Direct3Dデバイスコンテキスト
	ComPtr<IDXGISwapChain>			m_swapChain; // スワップチェイン
	ComPtr<ID3D11RenderTargetView>	m_renderTarget; // バックバッファーのRTビュー

	ComPtr<ID3D11Texture2D> m_depthStencilBuffer; // デプス&ステンシルバッファ
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;
};

inline std::unique_ptr <D3DManager> D3DManager::instance_ = nullptr;

} // UniDx
