#pragma once
#include "yaGraphics.h"

namespace ya
{
	class Image;
}

namespace ya::graphics
{
	// MohEngine(2023) Texture 에서 가져왔다. DirectXTex 대신 Image 가 가진 CPU 픽셀을 그대로 올린다.
	// Image 가 처음 그려질 때 만들어져 Image 와 함께 지워진다 (Image::GetTexture)
	class Texture
	{
	public:
		enum class eAlphaSource
		{
			ColorKey,	// 마젠타(255, 0, 255)는 (0, 0, 0, 0), 나머지는 불투명. 파일의 알파는 무시한다 (TransparentBlt 와 같다)
			ImageAlpha,	// 이미지의 알파(premultiplied)를 그대로 쓴다 (AlphaBlend + AC_SRC_ALPHA, PNG)
		};

		Texture();
		~Texture();

		bool Create(UINT width, UINT height, DXGI_FORMAT format, UINT bindFlag, const void* pixels = nullptr);
		// Image 픽셀을 premultiplied BGRA 로 바꿔 올린다. 크기가 같으면 있던 텍스처를 덮어쓴다
		bool Upload(Image* image, eAlphaSource source);
		// Image 가 SetPixel 등으로 바뀌었거나 알파를 다르게 써야 하면 false
		bool IsUpToDate(Image* image, eAlphaSource source);

		void BindShaderResource(eShaderStage stage, UINT startSlot);

		UINT GetWidth() { return mWidth; }
		UINT GetHeight() { return mHeight; }
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() { return mSRV; }
		Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() { return mTexture; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
		D3D11_TEXTURE2D_DESC mDesc;
		UINT mWidth;
		UINT mHeight;

		UINT mVersion;			// 올릴 때의 Image::GetVersion()
		eAlphaSource mSource;
		bool mbTried;			// 한 번이라도 올려 봤는지 (실패하면 mSRV 가 비어 있다)
	};
}
