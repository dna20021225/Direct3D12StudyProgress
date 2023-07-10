#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <wrl.h>
using namespace Microsoft::WRL;

class Shader
{
public:
	Shader(ID3D12Device* device, const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath,
		D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
		bool depthEnable = false, bool cullModeEnable = false, bool uvWrapEnable = true);
		bool isEnabled() const { return m_enabled; }
		void setShader(ID3D12GraphicsCommandList* cmdList);
	~Shader();

private:
	bool readShaderObject(const wchar_t* filePath, ID3DBlob** shaderObj);
	bool createRootSignature(ID3D12Device* device, bool uvWrapEnable);
	bool createPSO(ID3D12Device* device, ID3D10Blob* vertexShader, ID3D10Blob* pixelShader,
		D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
		bool depthEnable, bool cullModeEnable);

	bool m_enabled;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pso;
};
