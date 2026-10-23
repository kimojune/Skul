#include "yaRendererDX11.h"
#include "yaGraphicDevice_Dx11.h"
#include "yaConstantBuffer.h"
#include "yaMesh.h"
#include "yaShader.h"
#include "yaTexture.h"
#include "yaImage.h"

// 빌드할 때 FxCompile 이 Shaders/*.hlsl 을 컴파일해서 만든 바이트코드 배열 (g_SpriteVS, g_SpritePS)
#include "SpriteVS.hlsl.h"
#include "SpritePS.hlsl.h"

namespace ya
{
	RendererDX11::RendererDX11(bool forceWarp)
		: mbForceWarp(forceWarp)
		, mDevice(nullptr)
		, mSpriteShader(nullptr)
		, mRectMesh(nullptr)
		, mConstantBuffers{}
		, mFrameCB{}
		, mbPipelineBound(false)
		, mb2DDrawing(false)
		, mDummyHdc(nullptr)
		, mDummyBitmap(nullptr)
		, mDummyOldBitmap(nullptr)
	{
	}

	RendererDX11::~RendererDX11()
	{
		if (mb2DDrawing)
			mD2DContext->EndDraw();
		ReleaseCache();
		mBrush = nullptr;
		mD2DTarget = nullptr;
		mD2DContext = nullptr;
		mD2DDevice = nullptr;
		mD2DFactory = nullptr;
		mDWriteFactory = nullptr;

		for (graphics::ConstantBuffer*& buffer : mConstantBuffers)
		{
			delete buffer;
			buffer = nullptr;
		}
		delete mRectMesh;
		delete mSpriteShader;
		for (UINT i = 0; i < (UINT)graphics::eSamplerType::End; i++)
			mStates.samplerStates[i] = nullptr;
		for (UINT i = 0; i < (UINT)graphics::eRSType::End; i++)
			mStates.rasterizerStates[i] = nullptr;
		for (UINT i = 0; i < (UINT)graphics::eDSType::End; i++)
			mStates.depthStencilStates[i] = nullptr;
		for (UINT i = 0; i < (UINT)graphics::eBSType::End; i++)
			mStates.blendStates[i] = nullptr;

		if (graphics::GetDevice() == mDevice)
			graphics::GetDevice() = nullptr;
		delete mDevice;

		if (mDummyHdc != nullptr)
		{
			SelectObject(mDummyHdc, mDummyOldBitmap);
			DeleteObject(mDummyBitmap);
			DeleteDC(mDummyHdc);
		}
	}

	bool RendererDX11::Initialize(HWND hWnd, UINT width, UINT height)
	{
		mDevice = new graphics::GraphicDevice_Dx11();
		graphics::GetDevice() = mDevice;
		if (!mDevice->Initialize(hWnd, width, height, mbForceWarp))
			return false;

		if (!loadShader() || !setupState() || !loadMesh() || !loadBuffer())
			return false;

		if (!create2D())
			return false;

		// 카메라 이동은 그리는 쪽이 Camera::CaluatePos 로 이미 뺐으므로 View 는 단위 행렬이다.
		// Projection 은 화면 픽셀(왼쪽 위 0, 0 / 아래로 y 증가)을 NDC 로 바꾸는 직교 투영
		DirectX::XMStoreFloat4x4(&mFrameCB.view, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&mFrameCB.projection
			, DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)width, (float)height, 0.0f, 0.0f, 1.0f));
		mFrameCB.resolution = DirectX::XMFLOAT4((float)width, (float)height, 0.0f, 0.0f);

		HDC windowHdc = GetDC(hWnd);
		mDummyHdc = CreateCompatibleDC(windowHdc);
		mDummyBitmap = CreateCompatibleBitmap(windowHdc, 1, 1);
		mDummyOldBitmap = (HBITMAP)SelectObject(mDummyHdc, mDummyBitmap);
		ReleaseDC(hWnd, windowHdc);

		return true;
	}

	bool RendererDX11::loadShader()
	{
		mSpriteShader = new graphics::Shader();
		if (!mSpriteShader->Create(graphics::eShaderStage::VS, g_SpriteVS, sizeof(g_SpriteVS)))
			return false;
		if (!mSpriteShader->Create(graphics::eShaderStage::PS, g_SpritePS, sizeof(g_SpritePS)))
			return false;

		// Input layout 정점 구조 정보를 넘겨줘야한다.
		D3D11_INPUT_ELEMENT_DESC arrLayout[3] = {};

		arrLayout[0].AlignedByteOffset = 0;
		arrLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		arrLayout[0].InputSlot = 0;
		arrLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[0].SemanticName = "POSITION";
		arrLayout[0].SemanticIndex = 0;

		arrLayout[1].AlignedByteOffset = 12;
		arrLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		arrLayout[1].InputSlot = 0;
		arrLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[1].SemanticName = "COLOR";
		arrLayout[1].SemanticIndex = 0;

		arrLayout[2].AlignedByteOffset = 28;
		arrLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		arrLayout[2].InputSlot = 0;
		arrLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[2].SemanticName = "TEXCOORD";
		arrLayout[2].SemanticIndex = 0;

		if (!mSpriteShader->CreateInputLayout(arrLayout, 3))
			return false;

		mSpriteShader->SetRSState(graphics::eRSType::SolidNone);
		mSpriteShader->SetDSState(graphics::eDSType::None);
		mSpriteShader->SetBSState(graphics::eBSType::AlphaBlend);
		return true;
	}

	bool RendererDX11::setupState()
	{
		graphics::GraphicDevice_Dx11* device = graphics::GetDevice();
		bool result = true;

#pragma region Sampler State
		// 픽셀 아트라 점 샘플링. 시트 가장자리에서 반대편이 묻어나지 않게 CLAMP
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		result &= device->CreateSamplerState(&samplerDesc, mStates.samplerStates[(UINT)graphics::eSamplerType::Point].GetAddressOf());
#pragma endregion
#pragma region Rasterizer State
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.DepthClipEnable = TRUE;

		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		result &= device->CreateRasterizerState(&rasterizerDesc, mStates.rasterizerStates[(UINT)graphics::eRSType::SolidBack].GetAddressOf());

		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		result &= device->CreateRasterizerState(&rasterizerDesc, mStates.rasterizerStates[(UINT)graphics::eRSType::SolidNone].GetAddressOf());
#pragma endregion
#pragma region Depth Stencil State
		D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};

		//less
		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthstencildesc.StencilEnable = false;
		result &= device->CreateDepthStencilState(&depthstencildesc, mStates.depthStencilStates[(UINT)graphics::eDSType::Less].GetAddressOf());

		//None. 2D 는 그리는 순서가 곧 앞뒤라서 깊이를 쓰지 않는다
		depthstencildesc.DepthEnable = false;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthstencildesc.StencilEnable = false;
		result &= device->CreateDepthStencilState(&depthstencildesc, mStates.depthStencilStates[(UINT)graphics::eDSType::None].GetAddressOf());
#pragma endregion
#pragma region Blend State
		D3D11_BLEND_DESC blendDesc = {};

		//default
		mStates.blendStates[(UINT)graphics::eBSType::Default] = nullptr;

		// Alpha Blend. 텍스처가 premultiplied 라서 src 는 그대로(ONE) 더한다
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result &= device->CreateBlendState(&blendDesc, mStates.blendStates[(UINT)graphics::eBSType::AlphaBlend].GetAddressOf());
#pragma endregion

		return result;
	}

	bool RendererDX11::loadMesh()
	{
		// 단위 사각형. 화면 좌표는 y 가 아래로 커지므로 (0, 0) 이 왼쪽 위
		std::vector<graphics::Vertex> vertexes(4);
		std::vector<UINT> indexes = {};

		vertexes[0].pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertexes[0].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertexes[0].uv = DirectX::XMFLOAT2(0.0f, 0.0f);

		vertexes[1].pos = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		vertexes[1].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertexes[1].uv = DirectX::XMFLOAT2(1.0f, 0.0f);

		vertexes[2].pos = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f);
		vertexes[2].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertexes[2].uv = DirectX::XMFLOAT2(1.0f, 1.0f);

		vertexes[3].pos = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertexes[3].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertexes[3].uv = DirectX::XMFLOAT2(0.0f, 1.0f);

		indexes.push_back(0);
		indexes.push_back(1);
		indexes.push_back(2);

		indexes.push_back(0);
		indexes.push_back(2);
		indexes.push_back(3);

		mRectMesh = new graphics::Mesh();
		if (!mRectMesh->CreateVertexBuffer(vertexes.data(), (UINT)vertexes.size()))
			return false;
		if (!mRectMesh->CreateIndexBuffer(indexes.data(), (UINT)indexes.size()))
			return false;

		return true;
	}

	bool RendererDX11::loadBuffer()
	{
		mConstantBuffers[(UINT)graphics::eCBType::Frame] = new graphics::ConstantBuffer(graphics::eCBType::Frame);
		if (!mConstantBuffers[(UINT)graphics::eCBType::Frame]->Create(sizeof(graphics::FrameCB)))
			return false;

		mConstantBuffers[(UINT)graphics::eCBType::Sprite] = new graphics::ConstantBuffer(graphics::eCBType::Sprite);
		if (!mConstantBuffers[(UINT)graphics::eCBType::Sprite]->Create(sizeof(graphics::SpriteCB)))
			return false;

		return true;
	}

	bool RendererDX11::create2D()
	{
		D2D1_FACTORY_OPTIONS options = {};
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)mD2DFactory.GetAddressOf())))
			return false;

		// Direct3D 장치를 그대로 써서 Direct2D 장치를 만든다 (BGRA 지원으로 만든 장치라야 한다)
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice = nullptr;
		if (FAILED(mDevice->GetID3D11Device()->QueryInterface(__uuidof(IDXGIDevice), (void**)dxgiDevice.GetAddressOf())))
			return false;
		if (FAILED(mD2DFactory->CreateDevice(dxgiDevice.Get(), mD2DDevice.GetAddressOf())))
			return false;
		if (FAILED(mD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, mD2DContext.GetAddressOf())))
			return false;

		// 프레임 렌더 타깃 텍스처를 Direct2D 비트맵으로 감싸 그리기 대상으로 삼는다
		Microsoft::WRL::ComPtr<IDXGISurface> surface = nullptr;
		if (FAILED(mDevice->GetRenderTarget()->QueryInterface(__uuidof(IDXGISurface), (void**)surface.GetAddressOf())))
			return false;

		D2D1_BITMAP_PROPERTIES1 targetProperties = {};
		targetProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		targetProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		targetProperties.dpiX = 96.0f;
		targetProperties.dpiY = 96.0f;
		targetProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		if (FAILED(mD2DContext->CreateBitmapFromDxgiSurface(surface.Get(), &targetProperties, mD2DTarget.GetAddressOf())))
			return false;

		mD2DContext->SetTarget(mD2DTarget.Get());
		mD2DContext->SetDpi(96.0f, 96.0f);	// 1 DIP = 1 px
		// 사각형은 픽셀 경계에 딱 맞게(GDI FillRect 처럼), 글자는 회색조 안티에일리어싱(GDI ANTIALIASED_QUALITY 처럼)
		mD2DContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		mD2DContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

		D2D1_COLOR_F white = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (FAILED(mD2DContext->CreateSolidColorBrush(white, mBrush.GetAddressOf())))
			return false;

		if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)mDWriteFactory.GetAddressOf())))
			return false;

		return true;
	}

	void RendererDX11::BeginFrame(COLORREF clearColor)
	{
		FLOAT color[4] =
		{
			GetRValue(clearColor) / 255.0f,
			GetGValue(clearColor) / 255.0f,
			GetBValue(clearColor) / 255.0f,
			1.0f,
		};
		mDevice->ClearTarget(color);

		mConstantBuffers[(UINT)graphics::eCBType::Frame]->SetData(&mFrameCB);
		mbPipelineBound = false;
	}

	void RendererDX11::EndFrame()
	{
		end2D();
		mDevice->Present(false);
	}

	bool RendererDX11::ReadPixels(std::vector<BYTE>& pixels, UINT& width, UINT& height)
	{
		std::vector<BYTE> bgra;
		UINT rowPitch = 0;
		if (!mDevice->ReadRenderTarget(bgra, rowPitch))
			return false;

		// GDI 스크린샷과 같은 24비트 BMP 배치: 아래 줄부터, 한 줄은 4바이트 단위
		width = mDevice->GetWidth();
		height = mDevice->GetHeight();
		UINT bmpPitch = ((width * 3 + 3) / 4) * 4;
		pixels.assign((size_t)bmpPitch * height, 0);
		for (UINT y = 0; y < height; y++)
		{
			const BYTE* src = bgra.data() + (size_t)y * rowPitch;
			BYTE* dst = pixels.data() + (size_t)(height - 1 - y) * bmpPitch;
			for (UINT x = 0; x < width; x++)
			{
				dst[x * 3 + 0] = src[x * 4 + 0];
				dst[x * 3 + 1] = src[x * 4 + 1];
				dst[x * 3 + 2] = src[x * 4 + 2];
			}
		}
		return true;
	}

	void RendererDX11::begin3D()
	{
		end2D();
		if (!mbPipelineBound)
			bindPipeline();
	}

	void RendererDX11::begin2D()
	{
		if (mb2DDrawing)
			return;

		mD2DContext->BeginDraw();
		mb2DDrawing = true;
	}

	void RendererDX11::end2D()
	{
		if (!mb2DDrawing)
			return;

		// 모아 둔 Direct2D 그리기를 GPU 에 넘긴다. Direct2D 가 파이프라인 상태를 바꿨을 수 있으므로 다시 묶게 한다
		mD2DContext->EndDraw();
		mb2DDrawing = false;
		mbPipelineBound = false;
		mStats.drawCalls++;
	}

	void RendererDX11::bindPipeline()
	{
		mDevice->BindRenderTarget();
		mSpriteShader->Binds(mStates);
		mRectMesh->BindBuffer();

		mConstantBuffers[(UINT)graphics::eCBType::Frame]->Bind(graphics::eShaderStage::VS);
		mConstantBuffers[(UINT)graphics::eCBType::Sprite]->Bind(graphics::eShaderStage::VS);
		mConstantBuffers[(UINT)graphics::eCBType::Sprite]->Bind(graphics::eShaderStage::PS);
		mDevice->BindSampler(graphics::eShaderStage::PS, 0, mStates.samplerStates[(UINT)graphics::eSamplerType::Point].GetAddressOf());

		mbPipelineBound = true;
	}

	graphics::Texture* RendererDX11::prepareTexture(Image* image, eImageBlend blend)
	{
		graphics::Texture::eAlphaSource source = graphics::Texture::eAlphaSource::ColorKey;
		if (blend == eImageBlend::AlphaChannel || image->HasAlpha())
			source = graphics::Texture::eAlphaSource::ImageAlpha;

		// 처음 그리는 이미지면 텍스처를 만들고, 픽셀이 바뀌었으면 다시 올린다
		graphics::Texture* texture = image->GetTexture();
		if (texture == nullptr)
		{
			texture = new graphics::Texture();
			image->SetTexture(texture);
		}
		if (!texture->IsUpToDate(image, source))
			texture->Upload(image, source);

		return texture->GetSRV() != nullptr ? texture : nullptr;
	}

	void RendererDX11::DrawImage(Image* image, const RenderRect& dst, const RenderRect& src
		, eImageBlend blend, BYTE alpha, bool flipX)
	{
		graphics::Texture* texture = prepareTexture(image, blend);
		if (texture == nullptr)
			return;

		begin3D();

		graphics::SpriteCB spriteCB = {};
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling((float)dst.width, (float)dst.height, 1.0f)
			* DirectX::XMMatrixTranslation((float)dst.x, (float)dst.y, 0.0f);
		DirectX::XMStoreFloat4x4(&spriteCB.world, world);

		// 픽셀 셰이더가 화면 픽셀마다 원본 텍셀을 정수로 고르는 데 쓴다
		spriteCB.destRect = DirectX::XMFLOAT4((float)dst.x, (float)dst.y, (float)dst.width, (float)dst.height);
		spriteCB.sourceRect = DirectX::XMFLOAT4((float)src.x, (float)src.y, (float)src.width, (float)src.height);
		spriteCB.textureSize = DirectX::XMFLOAT2((float)texture->GetWidth(), (float)texture->GetHeight());

		// premultiplied 라서 전체 알파는 색과 알파 모두에 곱한다
		float tint = alpha / 255.0f;
		spriteCB.tint = DirectX::XMFLOAT4(tint, tint, tint, tint);
		spriteCB.flipX = flipX ? 1 : 0;

		mConstantBuffers[(UINT)graphics::eCBType::Sprite]->SetData(&spriteCB);
		texture->BindShaderResource(graphics::eShaderStage::PS, 0);
		mRectMesh->Render();
		mStats.drawCalls++;
	}

	void RendererDX11::setBrushColor(COLORREF color, BYTE alpha)
	{
		D2D1_COLOR_F brushColor =
		{
			GetRValue(color) / 255.0f,
			GetGValue(color) / 255.0f,
			GetBValue(color) / 255.0f,
			alpha / 255.0f,
		};
		mBrush->SetColor(brushColor);
	}

	void RendererDX11::FillRect(const RenderRect& rect, COLORREF color, BYTE alpha)
	{
		begin2D();
		setBrushColor(color, alpha);

		D2D1_RECT_F area = { (float)rect.x, (float)rect.y, (float)(rect.x + rect.width), (float)(rect.y + rect.height) };
		mD2DContext->FillRectangle(area, mBrush.Get());
	}

	void RendererDX11::DrawRect(const RenderRect& rect, COLORREF color, int thickness)
	{
		begin2D();
		setBrushColor(color, 255);

		// GDI Rectangle 처럼 펜의 가운데가 (x, y) ~ (x + width - 1, y + height - 1) 픽셀을 지나게 한다.
		// 홀수 두께는 픽셀 가운데, 짝수 두께는 픽셀 경계가 선의 가운데
		float offset = (thickness % 2 == 1) ? 0.5f : 0.0f;
		D2D1_RECT_F area =
		{
			rect.x + offset,
			rect.y + offset,
			rect.x + rect.width - 1 + offset,
			rect.y + rect.height - 1 + offset,
		};
		mD2DContext->DrawRectangle(area, mBrush.Get(), (float)thickness);
	}

	void RendererDX11::DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness)
	{
		begin2D();
		setBrushColor(color, 255);

		// 홀수 두께는 픽셀 가운데를 지나야 한 줄로 딱 맞는다
		float offset = (thickness % 2 == 1) ? 0.5f : 0.0f;
		D2D1_POINT_2F from = { x1 + offset, y1 + offset };
		D2D1_POINT_2F to = { x2 + offset, y2 + offset };
		mD2DContext->DrawLine(from, to, mBrush.Get(), (float)thickness);
	}

	IDWriteTextFormat* RendererDX11::getTextFormat(const RenderFont& font)
	{
		std::wstring key = std::wstring(font.face) + L"|" + std::to_wstring(font.size) + (font.bold ? L"|b" : L"");
		auto iter = mTextFormats.find(key);
		if (iter != mTextFormats.end())
			return iter->second.Get();

		// GDI 에서 높이를 -size 로 준 것과 같이 글자(em) 높이가 size px
		Microsoft::WRL::ComPtr<IDWriteTextFormat> format = nullptr;
		if (FAILED(mDWriteFactory->CreateTextFormat(font.face, nullptr
			, font.bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL
			, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL
			, (float)font.size, L"ko-KR", format.GetAddressOf())))
			return nullptr;

		format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		mTextFormats.insert(std::make_pair(key, format));
		return format.Get();
	}

	Microsoft::WRL::ComPtr<IDWriteTextLayout> RendererDX11::createTextLayout(const std::wstring& text, const RenderFont& font, float width, float height)
	{
		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout = nullptr;
		IDWriteTextFormat* format = getTextFormat(font);
		if (format != nullptr)
			mDWriteFactory->CreateTextLayout(text.c_str(), (UINT32)text.size(), format, width, height, layout.GetAddressOf());

		return layout;
	}

	void RendererDX11::DrawString(const std::wstring& text, int x, int y, const RenderFont& font, COLORREF color, eTextAlign align)
	{
		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout = createTextLayout(text, font, 8192.0f, 8192.0f);
		if (layout == nullptr)
			return;

		// GDI TA_CENTER, TA_RIGHT 처럼 x 를 가운데나 오른쪽 끝으로 맞춘다
		DWRITE_TEXT_METRICS metrics = {};
		layout->GetMetrics(&metrics);
		float left = (float)x;
		if (align == eTextAlign::Center)
			left -= floorf(metrics.widthIncludingTrailingWhitespace / 2.0f);
		else if (align == eTextAlign::Right)
			left -= ceilf(metrics.widthIncludingTrailingWhitespace);

		begin2D();
		setBrushColor(color, 255);
		D2D1_POINT_2F origin = { left, (float)y };
		mD2DContext->DrawTextLayout(origin, layout.Get(), mBrush.Get());
	}

	void RendererDX11::DrawStringBox(const std::wstring& text, const RenderRect& box, const RenderFont& font, COLORREF color, eTextAlign align)
	{
		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout = createTextLayout(text, font, (float)box.width, (float)box.height);
		if (layout == nullptr)
			return;

		// 한 줄, 세로 가운데 (GDI DT_SINGLELINE | DT_VCENTER). 상자 밖으로 나가도 자르지 않는다 (DT_NOCLIP)
		DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		if (align == eTextAlign::Center)
			textAlignment = DWRITE_TEXT_ALIGNMENT_CENTER;
		else if (align == eTextAlign::Right)
			textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
		layout->SetTextAlignment(textAlignment);
		layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		begin2D();
		setBrushColor(color, 255);
		D2D1_POINT_2F origin = { (float)box.x, (float)box.y };
		mD2DContext->DrawTextLayout(origin, layout.Get(), mBrush.Get());
	}

	SIZE RendererDX11::MeasureString(const std::wstring& text, const RenderFont& font)
	{
		SIZE size = {};
		Microsoft::WRL::ComPtr<IDWriteTextLayout> layout = createTextLayout(text, font, 8192.0f, 8192.0f);
		if (layout == nullptr)
			return size;

		DWRITE_TEXT_METRICS metrics = {};
		layout->GetMetrics(&metrics);
		size.cx = (LONG)ceilf(metrics.widthIncludingTrailingWhitespace);
		size.cy = (LONG)ceilf(metrics.height);
		return size;
	}

	void RendererDX11::ReleaseCache()
	{
		mTextFormats.clear();
	}

	std::wstring RendererDX11::GetAdapterName()
	{
		return mDevice != nullptr ? mDevice->GetAdapterName() : L"-";
	}
}
