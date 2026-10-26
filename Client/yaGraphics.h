#pragma once
#include "YamYamEngine.h"	// <windows.h> 를 먼저

#include <wrl.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// MohEngine(2023) yaGraphics.h 에서 가져왔다. 이 게임에 쓰는 것만 남겼다.
#define CB_GETBINDSLOT(name) __CBUFFERBINDSLOT__##name##__
#define CBUFFER(name, slot) static const int CB_GETBINDSLOT(name) = slot; struct alignas(16) name

#define CBSLOT_FRAME			0
#define CBSLOT_SPRITE			1

namespace ya::graphics
{
	enum class eShaderStage
	{
		VS,
		HS,
		DS,
		GS,
		PS,
		CS,
		End,
	};

	enum class eCBType
	{
		Frame,
		Sprite,
		End,
	};

	enum class eSamplerType
	{
		Point,
		End,
	};

	enum class eRSType
	{
		SolidBack,
		SolidNone,
		End,
	};

	enum class eDSType
	{
		Less,
		None,
		End,
	};

	enum class eBSType
	{
		Default,
		AlphaBlend,		// premultiplied: ONE, INV_SRC_ALPHA
		End,
	};

	struct GpuBuffer
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		D3D11_BUFFER_DESC desc;

		GpuBuffer()
			: buffer(nullptr)
			, desc()
		{
		}
		virtual ~GpuBuffer() = default;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 uv;
	};

	// Shaders/globals.hlsli 의 cbuffer 와 배치가 같아야 한다
	CBUFFER(FrameCB, CBSLOT_FRAME)
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 resolution;
	};

	CBUFFER(SpriteCB, CBSLOT_SPRITE)
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 destRect;
		DirectX::XMFLOAT4 sourceRect;
		DirectX::XMFLOAT4 tint;
		DirectX::XMFLOAT2 textureSize;
		UINT flipX;
		float flash;	// 0~1 흰색으로 섞는 정도
	};

	// 파이프라인 상태 묶음. RendererDX11 이 만들고 Shader::Binds 가 골라서 묶는다
	struct PipelineStates
	{
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[(UINT)eSamplerType::End];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[(UINT)eRSType::End];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[(UINT)eDSType::End];
		Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[(UINT)eBSType::End];
	};
}
