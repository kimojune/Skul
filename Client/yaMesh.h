#pragma once
#include "yaGraphics.h"

namespace ya::graphics
{
	// MohEngine(2023) Mesh 에서 가져왔다. 정점·인덱스 버퍼 한 쌍
	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		bool CreateVertexBuffer(void* data, UINT Count);
		bool CreateIndexBuffer(void* data, UINT Count);

		void BindBuffer();
		void Render();

		UINT GetIndexCount() { return mIndexCount; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

		D3D11_BUFFER_DESC mVBDesc;
		D3D11_BUFFER_DESC mIBDesc;

		UINT mIndexCount;
	};
}
