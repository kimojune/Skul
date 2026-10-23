#include "yaConstantBuffer.h"
#include "yaGraphicDevice_Dx11.h"

namespace ya::graphics
{
	ConstantBuffer::ConstantBuffer(const eCBType type)
		: GpuBuffer()
		, mType(type)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
	}

	bool ConstantBuffer::Create(size_t size)
	{
		desc.ByteWidth = (UINT)size;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		return GetDevice()->CreateBuffer(buffer.GetAddressOf(), &desc, nullptr);
	}

	void ConstantBuffer::SetData(void* data)
	{
		GetDevice()->SetConstantBuffer(buffer.Get(), data, desc.ByteWidth);
	}

	void ConstantBuffer::Bind(eShaderStage stage)
	{
		GetDevice()->BindConstantBuffer(stage, mType, buffer.Get());
	}
}
