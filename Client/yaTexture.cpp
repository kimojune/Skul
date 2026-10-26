#include "yaTexture.h"
#include "yaGraphicDevice_Dx11.h"
#include "yaImage.h"

namespace ya::graphics
{
	Texture::Texture()
		: mTexture(nullptr)
		, mSRV(nullptr)
		, mDesc{}
		, mWidth(0)
		, mHeight(0)
		, mVersion(0)
		, mSource(eAlphaSource::ColorKey)
		, mbTried(false)
	{
	}

	Texture::~Texture()
	{
	}

	bool Texture::Create(UINT width, UINT height, DXGI_FORMAT format, UINT bindflag, const void* pixels)
	{
		mTexture = nullptr;
		mSRV = nullptr;

		mDesc = {};
		mDesc.BindFlags = bindflag;
		mDesc.Usage = D3D11_USAGE_DEFAULT;
		mDesc.CPUAccessFlags = 0;
		mDesc.Format = format;
		mDesc.Width = width;
		mDesc.Height = height;
		mDesc.ArraySize = 1;

		mDesc.SampleDesc.Count = 1;
		mDesc.SampleDesc.Quality = 0;

		// 픽셀 아트는 축소해서 그리지 않으므로 밉맵 없이 한 장만
		mDesc.MipLevels = 1;
		mDesc.MiscFlags = 0;

		mWidth = width;
		mHeight = height;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = pixels;
		data.SysMemPitch = width * 4;

		if (!GetDevice()->CreateTexture2D(&mDesc, pixels != nullptr ? &data : nullptr, mTexture.GetAddressOf()))
			return false;

		if (bindflag & (UINT)D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
			tSRVDesc.Format = mDesc.Format;
			tSRVDesc.Texture2D.MipLevels = 1;
			tSRVDesc.Texture2D.MostDetailedMip = 0;
			tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

			if (!GetDevice()->CreateShaderResourceView(mTexture.Get(), &tSRVDesc, mSRV.GetAddressOf()))
				return false;
		}

		return true;
	}

	bool Texture::Upload(Image* image, eAlphaSource source)
	{
		// 실패해도 같은 이미지를 매 프레임 다시 읽지 않도록 시도한 판을 먼저 적어 둔다
		mVersion = image->GetVersion();
		mSource = source;
		mbTried = true;

		std::vector<BYTE> pixels;
		if (!image->ReadPixels(pixels))
		{
			mSRV = nullptr;
			return false;
		}

		// PNG 는 처음부터 premultiplied 알파가 있으므로 마젠타 키를 쓰지 않는다
		if (source == eAlphaSource::ColorKey && !image->HasAlpha())
		{
			for (size_t i = 0; i + 3 < pixels.size(); i += 4)
			{
				BYTE* bgra = &pixels[i];
				if (bgra[0] == 255 && bgra[1] == 0 && bgra[2] == 255)
				{
					bgra[0] = 0;
					bgra[1] = 0;
					bgra[2] = 0;
					bgra[3] = 0;
				}
				else
				{
					bgra[3] = 255;
				}
			}
		}

		UINT width = image->GetWidth();
		UINT height = image->GetHeight();
		if (mTexture != nullptr && mWidth == width && mHeight == height)
		{
			GetDevice()->GetID3D11DeviceContext()->UpdateSubresource(mTexture.Get(), 0, nullptr, pixels.data(), width * 4, 0);
		}
		else if (!Create(width, height, DXGI_FORMAT_B8G8R8A8_UNORM, D3D11_BIND_SHADER_RESOURCE, pixels.data()))
		{
			OutputDebugStringW((L"[Texture] cannot create " + std::to_wstring(width) + L"x" + std::to_wstring(height)
				+ L" texture for '" + image->GetPath() + L"'\n").c_str());
			mSRV = nullptr;
			return false;
		}

		return true;
	}

	bool Texture::IsUpToDate(Image* image, eAlphaSource source)
	{
		return mbTried
			&& mVersion == image->GetVersion()
			&& mSource == source;
	}

	void Texture::BindShaderResource(eShaderStage stage, UINT startSlot)
	{
		GetDevice()->BindShaderResource(stage, startSlot, mSRV.GetAddressOf());
	}
}
