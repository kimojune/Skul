#include "yaGraphicDevice_Dx11.h"

namespace ya::graphics
{
	GraphicDevice_Dx11::GraphicDevice_Dx11()
		: mViewPort{}
		, mFeatureLevel((D3D_FEATURE_LEVEL)0)
		, mWidth(0)
		, mHeight(0)
	{
	}

	GraphicDevice_Dx11::~GraphicDevice_Dx11()
	{
		if (mContext != nullptr)
		{
			mContext->ClearState();
			mContext->Flush();
		}
	}

	bool GraphicDevice_Dx11::Initialize(HWND hWnd, UINT width, UINT height, bool forceWarp)
	{
		mWidth = width;
		mHeight = height;

		//device, context 생성. 하드웨어가 안 되면 WARP(CPU)로 만든다
		if (forceWarp || !createDevice(D3D_DRIVER_TYPE_HARDWARE))
		{
			if (!createDevice(D3D_DRIVER_TYPE_WARP))
				return false;
		}

		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter = nullptr;
		if (SUCCEEDED(mDevice.As(&dxgiDevice)) && SUCCEEDED(dxgiDevice->GetAdapter(adapter.GetAddressOf())))
		{
			DXGI_ADAPTER_DESC adapterDesc = {};
			adapter->GetDesc(&adapterDesc);
			mAdapterName = adapterDesc.Description;
		}

		//SwapChain
		if (!CreateSwapChain(hWnd, width, height))
			return false;

		if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)mBackBuffer.GetAddressOf())))
			return false;

		// 한 프레임을 그리는 렌더 타깃. Direct2D 도 이 텍스처에 그린다
		D3D11_TEXTURE2D_DESC renderTargetDesc = {};
		renderTargetDesc.Width = width;
		renderTargetDesc.Height = height;
		renderTargetDesc.MipLevels = 1;
		renderTargetDesc.ArraySize = 1;
		renderTargetDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		renderTargetDesc.SampleDesc.Count = 1;
		renderTargetDesc.SampleDesc.Quality = 0;
		renderTargetDesc.Usage = D3D11_USAGE_DEFAULT;
		renderTargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		if (!CreateTexture2D(&renderTargetDesc, nullptr, mRenderTarget.GetAddressOf()))
			return false;

		if (!CreateRenderTargetView(mRenderTarget.Get(), nullptr, mRenderTargetView.GetAddressOf()))
			return false;

		mViewPort =
		{
			0.0f, 0.0f
			, (float)width
			, (float)height
			, 0.0f, 1.0f
		};

		BindViewPort(&mViewPort);
		BindRenderTarget();
		return true;
	}

	bool GraphicDevice_Dx11::createDevice(D3D_DRIVER_TYPE driverType)
	{
		// Direct2D 가 같은 장치에 그리려면 BGRA 지원이 있어야 한다
		UINT flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		HRESULT hr = D3D11CreateDevice(nullptr, driverType, nullptr, flag
			, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION
			, mDevice.ReleaseAndGetAddressOf(), &mFeatureLevel, mContext.ReleaseAndGetAddressOf());

		// 11_1 을 모르는 런타임은 E_INVALIDARG 를 돌려준다
		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDevice(nullptr, driverType, nullptr, flag
				, featureLevels + 1, _countof(featureLevels) - 1, D3D11_SDK_VERSION
				, mDevice.ReleaseAndGetAddressOf(), &mFeatureLevel, mContext.ReleaseAndGetAddressOf());
		}

		return SUCCEEDED(hr);
	}

	bool GraphicDevice_Dx11::CreateSwapChain(HWND hWnd, UINT width, UINT height)
	{
		Microsoft::WRL::ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory2> pFactory = nullptr;

		if (FAILED(mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf())))
			return false;

		if (FAILED(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf())))
			return false;

		if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)pFactory.GetAddressOf())))
			return false;

		DXGI_SWAP_CHAIN_DESC1 dxgiDesc = {};
		dxgiDesc.Width = width;
		dxgiDesc.Height = height;
		dxgiDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		dxgiDesc.SampleDesc.Count = 1;
		dxgiDesc.SampleDesc.Quality = 0;
		dxgiDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		dxgiDesc.BufferCount = 2;
		dxgiDesc.Scaling = DXGI_SCALING_STRETCH;		// 창 크기가 달라지면 늘려서 보여준다
		dxgiDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		dxgiDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		HRESULT hr = pFactory->CreateSwapChainForHwnd(mDevice.Get(), hWnd, &dxgiDesc, nullptr, nullptr, mSwapChain.GetAddressOf());
		if (FAILED(hr))
		{
			// Windows 10 이전은 flip discard 를 모른다
			dxgiDesc.BufferCount = 1;
			dxgiDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			hr = pFactory->CreateSwapChainForHwnd(mDevice.Get(), hWnd, &dxgiDesc, nullptr, nullptr, mSwapChain.GetAddressOf());
		}
		if (FAILED(hr))
			return false;

		// Alt+Enter 전체 화면 전환은 쓰지 않는다
		pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
		return true;
	}

	bool GraphicDevice_Dx11::CreateTexture2D(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* data, ID3D11Texture2D** ppTexture2D)
	{
		if (FAILED(mDevice->CreateTexture2D(desc, data, ppTexture2D)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs
		, UINT NumElements
		, const void* byteCode, SIZE_T byteCodeLength
		, ID3D11InputLayout** ppInputLayout)
	{
		if (FAILED(mDevice->CreateInputLayout(pInputElementDescs, NumElements, byteCode, byteCodeLength, ppInputLayout)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateBuffer(ID3D11Buffer** buffer, D3D11_BUFFER_DESC* desc, D3D11_SUBRESOURCE_DATA* data)
	{
		if (FAILED(mDevice->CreateBuffer(desc, data, buffer)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateVertexShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11VertexShader** ppVertexShader)
	{
		if (FAILED(mDevice->CreateVertexShader(pShaderBytecode, BytecodeLength, nullptr, ppVertexShader)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreatePixelShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11PixelShader** ppPixelShader)
	{
		if (FAILED(mDevice->CreatePixelShader(pShaderBytecode, BytecodeLength, nullptr, ppPixelShader)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState)
	{
		if (FAILED(mDevice->CreateSamplerState(pSamplerDesc, ppSamplerState)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** ppRasterizerState)
	{
		if (FAILED(mDevice->CreateRasterizerState(pRasterizerDesc, ppRasterizerState)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** ppDepthStencilState)
	{
		if (FAILED(mDevice->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateBlendState(const D3D11_BLEND_DESC* pBlendStateDesc, ID3D11BlendState** ppBlendState)
	{
		if (FAILED(mDevice->CreateBlendState(pBlendStateDesc, ppBlendState)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView)
	{
		if (FAILED(mDevice->CreateShaderResourceView(pResource, pDesc, ppSRView)))
			return false;

		return true;
	}

	bool GraphicDevice_Dx11::CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView)
	{
		if (FAILED(mDevice->CreateRenderTargetView(pResource, pDesc, ppRTView)))
			return false;

		return true;
	}

	void GraphicDevice_Dx11::BindViewPort(D3D11_VIEWPORT* viewport)
	{
		mContext->RSSetViewports(1, viewport);
	}

	void GraphicDevice_Dx11::BindRasterizeState(ID3D11RasterizerState* pRasterizerState)
	{
		mContext->RSSetState(pRasterizerState);
	}

	void GraphicDevice_Dx11::BindDepthStencilState(ID3D11DepthStencilState* pDepthStencilState)
	{
		mContext->OMSetDepthStencilState(pDepthStencilState, 0);
	}

	void GraphicDevice_Dx11::BindBlendState(ID3D11BlendState* pBlendState)
	{
		mContext->OMSetBlendState(pBlendState, nullptr, 0xffffffff);
	}

	void GraphicDevice_Dx11::BindRenderTarget()
	{
		mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
		mContext->RSSetViewports(1, &mViewPort);
	}

	void GraphicDevice_Dx11::BindInputLayout(ID3D11InputLayout* pInputLayout)
	{
		mContext->IASetInputLayout(pInputLayout);
	}

	void GraphicDevice_Dx11::BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
	{
		mContext->IASetPrimitiveTopology(Topology);
	}

	void GraphicDevice_Dx11::BindVertexBuffer(UINT StartSlot, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets)
	{
		mContext->IASetVertexBuffers(StartSlot, 1, ppVertexBuffers, pStrides, pOffsets);
	}

	void GraphicDevice_Dx11::BindIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
	{
		mContext->IASetIndexBuffer(pIndexBuffer, Format, Offset);
	}

	void GraphicDevice_Dx11::BindVertexShader(ID3D11VertexShader* pVetexShader)
	{
		mContext->VSSetShader(pVetexShader, 0, 0);
	}

	void GraphicDevice_Dx11::BindPixelShader(ID3D11PixelShader* pPixelShader)
	{
		mContext->PSSetShader(pPixelShader, 0, 0);
	}

	void GraphicDevice_Dx11::SetConstantBuffer(ID3D11Buffer* buffer, void* data, UINT size)
	{
		D3D11_MAPPED_SUBRESOURCE subRes = {};
		if (FAILED(mContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
			return;
		memcpy(subRes.pData, data, size);
		mContext->Unmap(buffer, 0);
	}

	void GraphicDevice_Dx11::BindConstantBuffer(eShaderStage stage, eCBType type, ID3D11Buffer* buffer)
	{
		switch (stage)
		{
		case eShaderStage::VS:
			mContext->VSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case eShaderStage::HS:
			mContext->HSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case eShaderStage::DS:
			mContext->DSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case eShaderStage::GS:
			mContext->GSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case eShaderStage::PS:
			mContext->PSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case eShaderStage::CS:
			mContext->CSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		default:
			break;
		}
	}

	void GraphicDevice_Dx11::BindShaderResource(eShaderStage stage, UINT startSlot, ID3D11ShaderResourceView** ppSRV)
	{
		switch (stage)
		{
		case eShaderStage::VS:
			mContext->VSSetShaderResources(startSlot, 1, ppSRV);
			break;
		case eShaderStage::PS:
			mContext->PSSetShaderResources(startSlot, 1, ppSRV);
			break;
		case eShaderStage::CS:
			mContext->CSSetShaderResources(startSlot, 1, ppSRV);
			break;
		default:
			break;
		}
	}

	void GraphicDevice_Dx11::BindSampler(eShaderStage stage, UINT StartSlot, ID3D11SamplerState** ppSamplers)
	{
		switch (stage)
		{
		case eShaderStage::VS:
			mContext->VSSetSamplers(StartSlot, 1, ppSamplers);
			break;
		case eShaderStage::PS:
			mContext->PSSetSamplers(StartSlot, 1, ppSamplers);
			break;
		case eShaderStage::CS:
			mContext->CSSetSamplers(StartSlot, 1, ppSamplers);
			break;
		default:
			break;
		}
	}

	void GraphicDevice_Dx11::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		mContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	}

	void GraphicDevice_Dx11::ClearTarget(const FLOAT color[4])
	{
		mContext->ClearRenderTargetView(mRenderTargetView.Get(), color);
		BindRenderTarget();
	}

	void GraphicDevice_Dx11::Present(bool vsync)
	{
		// 다 그린 렌더 타깃을 스왑체인 백버퍼로 복사해서 내보낸다
		mContext->CopyResource(mBackBuffer.Get(), mRenderTarget.Get());
		mSwapChain->Present(vsync ? 1 : 0, 0);
	}

	bool GraphicDevice_Dx11::ReadRenderTarget(std::vector<BYTE>& bgra, UINT& rowPitch)
	{
		if (mStaging == nullptr)
		{
			D3D11_TEXTURE2D_DESC stagingDesc = {};
			mRenderTarget->GetDesc(&stagingDesc);
			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.BindFlags = 0;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			stagingDesc.MiscFlags = 0;

			if (!CreateTexture2D(&stagingDesc, nullptr, mStaging.GetAddressOf()))
				return false;
		}

		// GPU -> CPU. Map 이 GPU 가 다 그릴 때까지 기다린다
		mContext->CopyResource(mStaging.Get(), mRenderTarget.Get());

		D3D11_MAPPED_SUBRESOURCE mapped = {};
		if (FAILED(mContext->Map(mStaging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
			return false;

		rowPitch = mWidth * 4;
		bgra.resize((size_t)rowPitch * mHeight);
		for (UINT y = 0; y < mHeight; y++)
		{
			memcpy(bgra.data() + (size_t)y * rowPitch, (BYTE*)mapped.pData + (size_t)y * mapped.RowPitch, rowPitch);
		}

		mContext->Unmap(mStaging.Get(), 0);
		return true;
	}
}
