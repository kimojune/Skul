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

		// 이미지를 그리기 전에 파이프라인을 묶는다 (프레임 처음, Direct2D 를 쓴 뒤)
		void begin3D();
		void bindPipeline();
		graphics::Texture* prepareTexture(Image* image, eImageBlend blend);

		bool mbForceWarp;
		graphics::GraphicDevice_Dx11* mDevice;
		graphics::Shader* mSpriteShader;
		graphics::Mesh* mRectMesh;
		graphics::ConstantBuffer* mConstantBuffers[(UINT)graphics::eCBType::End];
		graphics::PipelineStates mStates;
		graphics::FrameCB mFrameCB;
		bool mbPipelineBound;

		// Render(HDC) 에 넘길 DC. 화면에 나오지 않는다
		HDC mDummyHdc;
		HBITMAP mDummyBitmap;
		HBITMAP mDummyOldBitmap;
	};
}
