#include "Shader.h"

#include <vector>
#include <d3d11.h>

#include "D3DManager.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace UniDx;

bool Shader::Compile(const wchar_t* filePath)
{
	// 頂点シェーダーを読み込み＆コンパイル
	ComPtr<ID3DBlob> compiledVS;
	if (FAILED(D3DCompileFromFile(filePath, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &compiledVS, nullptr)))
	{
		OutputDebugStringW(L"頂点シェーダーのコンパイルエラー");
		return false;
	}
	// ピクセルシェーダーを読み込み＆コンパイル
	ComPtr<ID3DBlob> compiledPS;
	if (FAILED(D3DCompileFromFile(filePath, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &compiledPS, nullptr)))
	{
		OutputDebugStringW(L"ピクセルシェーダーシェーダーのコンパイルエラー");
		return false;
	}

	// 頂点シェーダー作成
	if (FAILED(D3DManager::getInstance()->GetDevice()->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, &m_vertex)))
	{
		OutputDebugStringW(L"頂点シェーダーの作成エラー");
		return false;
	}
	// ピクセルシェーダー作成
	if (FAILED(D3DManager::getInstance()->GetDevice()->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &m_pixel)))
	{
		OutputDebugStringW(L"ピクセルシェーダーの作成エラー");
		return false;
	}

	// １頂点の詳細な情報
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点インプットレイアウト作成
	if (FAILED(D3DManager::getInstance()->GetDevice()->CreateInputLayout(&layout[0], UINT(layout.size()), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &m_inputLayout)))
	{
		OutputDebugStringW(L"頂点インプットレイアウトの作成エラー");
		return false;
	}
	OutputDebugStringW(L"成功");

	return true;
}

void Shader::SetToContext()
{
	D3DManager::getInstance()->GetContext()->VSSetShader(m_vertex.Get(), 0, 0);
	D3DManager::getInstance()->GetContext()->PSSetShader(m_pixel.Get(), 0, 0);
	D3DManager::getInstance()->GetContext()->IASetInputLayout(m_inputLayout.Get());
}