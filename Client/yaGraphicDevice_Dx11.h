#pragma once
#include "yaGraphics.h"

namespace ya::graphics
{
	// MohEngine(2023) GraphicDevice_Dx11 을 가져와 이 게임에 맞게 바꿨다.
	//  - 하드웨어 장치를 못 만들면 WARP(소프트웨어) 장치로 만든다
	//  - BGRA 스왑체인 (Direct2D 와 같은 표면을 쓰기 위해 D3D11_CREATE_DEVICE_BGRA_SUPPORT)
	//  - 깊이 버퍼 없음. 한 프레임은 화면 크기 렌더 타깃에 그리고 Present 직전에 백버퍼로 복사한다
	//    (Present 뒤에도 스크린샷을 그 렌더 타깃에서 읽을 수 있다)
	class GraphicDevice_Dx11
	{
	public:
		GraphicDevice_Dx11();
		~GraphicDevice_Dx11();

		bool Initialize(HWND hWnd, UINT width, UINT height, bool forceWarp);

		bool CreateSwapChain(HWND hWnd, UINT width, UINT height);
		bool CreateTexture2D(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* data, ID3D11Texture2D** ppTexture2D);
		bool CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, const void* byteCode, SIZE_T byteCodeLength, ID3D11InputLayout** ppInputLayout);
		bool CreateBuffer(ID3D11Buffer** buffer, D3D11_BUFFER_DESC* desc, D3D11_SUBRESOURCE_DATA* data);
		bool CreateVertexShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11VertexShader** ppVertexShader);
		bool CreatePixelShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11PixelShader** ppPixelShader);
		bool CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState);
		bool CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState);
		bool CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** ppDepthStencilState);
		bool CreateBlendState(const D3D11_BLEND_DESC* pBlendStateDesc, ID3D11BlendState** ppBlendState);
		bool CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView);
		bool CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView);

		void BindInputLayout(ID3D11InputLayout* pInputLayout);
		void BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);
		void BindVertexBuffer(UINT StartSlot, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets);
		void BindIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset);
		void BindVertexShader(ID3D11VertexShader* pVetexShader);
		void BindPixelShader(ID3D11PixelShader* pPixelShader);

		void SetConstantBuffer(ID3D11Buffer* buffer, void* data, UINT size);
		void BindConstantBuffer(eShaderStage stage, eCBType type, ID3D11Buffer* buffer);
		void BindShaderResource(eShaderStage stage, UINT startSlot, ID3D11ShaderResourceView** ppSRV);
		void BindSampler(eShaderStage stage, UINT StartSlot, ID3D11SamplerState** ppSamplers);
		void BindViewPort(D3D11_VIEWPORT* viewport);
		void BindRasterizeState(ID3D11RasterizerState* pRasterizerState);
		void BindDepthStencilState(ID3D11DepthStencilState* pDepthStencilState);
		void BindBlendState(ID3D11BlendState* pBlendState);
		void BindRenderTarget();

		void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		void ClearTarget(const FLOAT color[4]);
		void Present(bool vsync);
		// 렌더 타깃을 CPU로 읽는다. 위 줄부터 BGRA, 한 줄은 rowPitch 바이트
		bool ReadRenderTarget(std::vector<BYTE>& bgra, UINT& rowPitch);

		ID3D11Device* GetID3D11Device() { return mDevice.Get(); }
		ID3D11DeviceContext* GetID3D11DeviceContext() { return mContext.Get(); }
		ID3D11Texture2D* GetRenderTarget() { return mRenderTarget.Get(); }
		const std::wstring& GetAdapterName() { return mAdapterName; }
		D3D_FEATURE_LEVEL GetFeatureLevel() { return mFeatureLevel; }
		UINT GetWidth() { return mWidth; }
		UINT GetHeight() { return mHeight; }

	private:
		bool createDevice(D3D_DRIVER_TYPE driverType);

		Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mBackBuffer;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mStaging;
		D3D11_VIEWPORT mViewPort;
		D3D_FEATURE_LEVEL mFeatureLevel;
		std::wstring mAdapterName;
		UINT mWidth;
		UINT mHeight;
	};

	inline GraphicDevice_Dx11*& GetDevice()
	{
		static GraphicDevice_Dx11* device = nullptr;
		return device;
	}
}
