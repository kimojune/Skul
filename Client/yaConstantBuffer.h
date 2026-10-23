#pragma once
#include "yaGraphics.h"

namespace ya::graphics
{
	// MohEngine(2023) ConstantBuffer 그대로. CPU 에서 매번 덮어쓰는 동적 상수 버퍼
	class ConstantBuffer : public GpuBuffer
	{
	public:
		ConstantBuffer(const eCBType type);
		~ConstantBuffer();

		bool Create(size_t size);
		void SetData(void* data);
		void Bind(eShaderStage stage);

	private:
		const eCBType mType;
	};
}
