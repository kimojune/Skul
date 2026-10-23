#pragma once
#include "yaGraphics.h"

namespace ya::graphics
{
	// MohEngine(2023) Shader 에서 가져왔다.
	// MohEngine 은 실행할 때 .hlsl 을 컴파일했지만, 여기서는 빌드할 때 FxCompile 이 헤더에 넣어 준
	// 바이트코드로 만든다 (실행 위치와 상관없이 셰이더 파일을 찾을 필요가 없다)
	class Shader
	{
	public:
		Shader();
		~Shader();

		bool Create(const eShaderStage stage, const void* byteCode, SIZE_T byteCodeLength);
		bool CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements);
		void Binds(const PipelineStates& states);

		void SetRSState(eRSType type) { mRSType = type; }
		void SetDSState(eDSType type) { mDSType = type; }
		void SetBSState(eBSType type) { mBSType = type; }
		void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { mTopology = topology; }

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		D3D11_PRIMITIVE_TOPOLOGY mTopology;

		const void* mVSCode;
		SIZE_T mVSCodeLength;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPS;

		eRSType mRSType;
		eDSType mDSType;
		eBSType mBSType;
	};
}
