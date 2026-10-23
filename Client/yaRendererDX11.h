#pragma once
#include "yaRendererBackend.h"
#include "yaGraphics.h"

namespace ya::graphics
{
	class GraphicDevice_Dx11;
	class ConstantBuffer;
	class Mesh;
	class Shader;
	class Texture;
}

namespace ya
{
	// Direct3D 11 백엔드. 이미지 한 장을 이렇게 그린다.
	//  입력 조립기: 단위 사각형(0~1) 정점 4개, 인덱스 6개
	//  정점 셰이더: World(사각형 -> 화면 픽셀) x View(단위) x Projection(픽셀 -> NDC)
	//  래스터라이저: 컬링 없음
	//  픽셀 셰이더: 화면 픽셀마다 원본 텍셀을 정수로 골라(GDI StretchBlt 와 같은 규칙) 점 샘플링,
	//              premultiplied 색 x 전체 알파
	//  출력 병합기: ONE, INV_SRC_ALPHA 블렌드, 깊이 없음
	// 글자·사각형·선(UI, 디버그 그림, 화면 페이드)은 같은 렌더 타깃에 Direct2D + DirectWrite 로 그린다.
	// 둘을 섞어 불러도 부른 순서대로 그려지도록 바뀔 때마다 Direct2D 의 BeginDraw/EndDraw 를 열고 닫는다.
	class RendererDX11 : public RendererBackend
	{
	public:
		RendererDX11(bool forceWarp);
		virtual ~RendererDX11();

		virtual bool Initialize(HWND hWnd, UINT width, UINT height) override;
		virtual void BeginFrame(COLORREF clearColor) override;
		virtual void EndFrame() override;
		virtual bool ReadPixels(std::vector<BYTE>& pixels, UINT& width, UINT& height) override;

		virtual void DrawImage(Image* image, const RenderRect& dst, const RenderRect& src
			, eImageBlend blend, BYTE alpha, bool flipX) override;
		virtual void FillRect(const RenderRect& rect, COLORREF color, BYTE alpha) override;
		virtual void DrawRect(const RenderRect& rect, COLORREF color, int thickness) override;
		virtual void DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness) override;
		virtual void DrawString(const std::wstring& text, int x, int y, const RenderFont& font, COLORREF color, eTextAlign align) override;
		virtual void DrawStringBox(const std::wstring& text, const RenderRect& box, const RenderFont& font, COLORREF color, eTextAlign align) override;
		virtual SIZE MeasureString(const std::wstring& text, const RenderFont& font) override;
		virtual void ReleaseCache() override;

		virtual HDC GetHdc() override { return mDummyHdc; }
		virtual const wchar_t* GetName() override { return L"DX11"; }
		virtual std::wstring GetAdapterName() override;

	private:
		// MohEngine renderer::Initialize 순서: 셰이더 -> 상태 -> 메쉬 -> 상수 버퍼
		bool loadShader();
		bool setupState();
		bool loadMesh();
		bool loadBuffer();

		bool create2D();

		// 이미지를 그리기 전에 파이프라인을 묶는다 (프레임 처음, Direct2D 를 쓴 뒤)
		void begin3D();
		void bindPipeline();
		// Direct2D 그리기를 열고 닫는다. 닫으면 Direct3D 상태를 다시 묶어야 한다
		void begin2D();
		void end2D();
		graphics::Texture* prepareTexture(Image* image, eImageBlend blend);
		IDWriteTextFormat* getTextFormat(const RenderFont& font);
		Microsoft::WRL::ComPtr<IDWriteTextLayout> createTextLayout(const std::wstring& text, const RenderFont& font, float width, float height);
		void setBrushColor(COLORREF color, BYTE alpha);

		bool mbForceWarp;
		graphics::GraphicDevice_Dx11* mDevice;
		graphics::Shader* mSpriteShader;
		graphics::Mesh* mRectMesh;
		graphics::ConstantBuffer* mConstantBuffers[(UINT)graphics::eCBType::End];
		graphics::PipelineStates mStates;
		graphics::FrameCB mFrameCB;
		bool mbPipelineBound;

		// Direct2D + DirectWrite. 같은 D3D 장치에서 만들고 같은 렌더 타깃에 그린다
		Microsoft::WRL::ComPtr<ID2D1Factory1> mD2DFactory;
		Microsoft::WRL::ComPtr<ID2D1Device> mD2DDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> mD2DContext;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1> mD2DTarget;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> mBrush;
		Microsoft::WRL::ComPtr<IDWriteFactory> mDWriteFactory;
		std::map<std::wstring, Microsoft::WRL::ComPtr<IDWriteTextFormat>> mTextFormats;
		bool mb2DDrawing;

		// Render(HDC) 에 넘길 DC. 화면에 나오지 않는다
		HDC mDummyHdc;
		HBITMAP mDummyBitmap;
		HBITMAP mDummyOldBitmap;
	};
}
