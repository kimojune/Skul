#include "yaShader.h"
#include "yaGraphicDevice_Dx11.h"

namespace ya::graphics
{
	Shader::Shader()
		: mInputLayout(nullptr)
		, mTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		, mVSCode(nullptr)
		, mVSCodeLength(0)
		, mRSType(eRSType::SolidNone)
		, mDSType(eDSType::None)
		, mBSType(eBSType::AlphaBlend)
	{
	}

	Shader::~Shader()
	{
	}

	bool Shader::Create(const eShaderStage stage, const void* byteCode, SIZE_T byteCodeLength)
	{
		if (stage == eShaderStage::VS)
		{
			// 입력 레이아웃은 정점 셰이더 바이트코드로 검사하므로 남겨 둔다
			mVSCode = byteCode;
			mVSCodeLength = byteCodeLength;
			return GetDevice()->CreateVertexShader(byteCode, byteCodeLength, mVS.GetAddressOf());
		}
		else if (stage == eShaderStage::PS)
		{
			return GetDevice()->CreatePixelShader(byteCode, byteCodeLength, mPS.GetAddressOf());
		}

		return false;
	}

	bool Shader::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements)
	{
		if (mVSCode == nullptr)
			return false;

		return GetDevice()->CreateInputLayout(pInputElementDescs, NumElements
			, mVSCode, mVSCodeLength, mInputLayout.ReleaseAndGetAddressOf());
	}

	void Shader::Binds(const PipelineStates& states)
	{
		GetDevice()->BindInputLayout(mInputLayout.Get());
		GetDevice()->BindPrimitiveTopology(mTopology);

		GetDevice()->BindVertexShader(mVS.Get());
		GetDevice()->BindPixelShader(mPS.Get());

		GetDevice()->BindRasterizeState(states.rasterizerStates[(UINT)mRSType].Get());
		GetDevice()->BindDepthStencilState(states.depthStencilStates[(UINT)mDSType].Get());
		GetDevice()->BindBlendState(states.blendStates[(UINT)mBSType].Get());
	}
}
