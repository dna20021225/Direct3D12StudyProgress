#include "Shader.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib") 

Shader::Shader(ID3D12Device* device, const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath,
	D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
	bool depthEnable, bool cullModeEnable, bool uvWrapEnable)
	:m_enabled(false)
{
	ComPtr<ID3D10Blob> vsBlob;
	ComPtr<ID3D10Blob> psBlob;

	if (!readShaderObject(vertexShaderPath, vsBlob.GetAddressOf())) return;
	if (!readShaderObject(pixelShaderPath, psBlob.GetAddressOf())) return;
	if (!createRootSignature(device, uvWrapEnable))return;
	if (!createPSO(device, vsBlob.Get(), psBlob.Get(), inputLayouts, layoutNum, depthEnable, cullModeEnable)) return;

	m_enabled = true;
}

Shader::~Shader()
{
}

bool Shader::readShaderObject(const wchar_t* filePath, ID3DBlob** shaderObj)
{
	HRESULT hr = D3DReadFileToBlob(filePath, shaderObj);

	return SUCCEEDED(hr);
}

bool Shader::createRootSignature(ID3D12Device* device, bool uvWrapEnable)
{
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE dRange[2] = {};
	dRange[0].NumDescriptors = 1;
	dRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dRange[0].BaseShaderRegister = 0;
	dRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	dRange[1].NumDescriptors = 1;
	dRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dRange[1].BaseShaderRegister = 0;
	dRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rParam[2] = {};
	rParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rParam[0].DescriptorTable.pDescriptorRanges = &dRange[0];
	rParam[0].DescriptorTable.NumDescriptorRanges = 1;

	rParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rParam[1].DescriptorTable.pDescriptorRanges = &dRange[1];
	rParam[1].DescriptorTable.NumDescriptorRanges = 1;

	desc.pParameters = rParam;
	desc.NumParameters = 2;

	D3D12_TEXTURE_ADDRESS_MODE address_mode = (uvWrapEnable) ?
		D3D12_TEXTURE_ADDRESS_MODE_WRAP : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = address_mode;
	samplerDesc.AddressV = address_mode;
	samplerDesc.AddressW = address_mode;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	desc.pStaticSamplers = &samplerDesc;
	desc.NumStaticSamplers = 1;

	ComPtr<ID3DBlob> blob;
	HRESULT hr = D3D12SerializeRootSignature(
		&desc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		blob.GetAddressOf(), nullptr);
	if (FAILED(hr)) return false;

	hr = device->CreateRootSignature(
		0, blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_PPV_ARGS(m_rootSignature.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Shader::createPSO(ID3D12Device* device, ID3D10Blob* vertexShader, ID3D10Blob* pixelShader,
	D3D12_INPUT_ELEMENT_DESC* inputLayouts, UINT layoutNum,
	bool depthEnable, bool cullModeEnable)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pDesc = {};

	pDesc.VS.pShaderBytecode = vertexShader->GetBufferPointer();
	pDesc.VS.BytecodeLength  = vertexShader->GetBufferSize();
	pDesc.PS.pShaderBytecode = pixelShader->GetBufferPointer();
	pDesc.PS.BytecodeLength  = pixelShader->GetBufferSize();

	pDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	D3D12_RENDER_TARGET_BLEND_DESC rtDesc = {};
	rtDesc.BlendEnable				= FALSE;
	rtDesc.RenderTargetWriteMask	= D3D12_COLOR_WRITE_ENABLE_ALL;
	rtDesc.LogicOpEnable			= FALSE;

	pDesc.BlendState.AlphaToCoverageEnable	= FALSE;
	pDesc.BlendState.IndependentBlendEnable = FALSE;
	pDesc.BlendState.RenderTarget[0]		= rtDesc;

	pDesc.RasterizerState.MultisampleEnable		= FALSE;
	pDesc.RasterizerState.CullMode	= (cullModeEnable) ? D3D12_CULL_MODE_BACK : D3D12_CULL_MODE_NONE;
	pDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pDesc.RasterizerState.DepthClipEnable		= TRUE;
	pDesc.RasterizerState.FrontCounterClockwise = FALSE;
	pDesc.RasterizerState.DepthBias				= D3D12_DEFAULT_DEPTH_BIAS;
	pDesc.RasterizerState.DepthBiasClamp		= D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	pDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	pDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	pDesc.RasterizerState.ForcedSampleCount		= 0;
	pDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	if (depthEnable)
	{
		pDesc.DepthStencilState.DepthEnable		= TRUE;
		pDesc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK_ALL;
		pDesc.DepthStencilState.DepthFunc		= D3D12_COMPARISON_FUNC_LESS;
		pDesc.DSVFormat							= DXGI_FORMAT_D32_FLOAT;
	}
	else
	{
		pDesc.DepthStencilState.DepthEnable = FALSE;

	}
	pDesc.DepthStencilState.StencilEnable = FALSE;

	pDesc.InputLayout.pInputElementDescs	= inputLayouts;
	pDesc.InputLayout.NumElements			= layoutNum;

	pDesc.IBStripCutValue		= D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	pDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pDesc.NumRenderTargets		= 1;
	pDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;

	pDesc.SampleDesc.Count		= 1;
	pDesc.SampleDesc.Quality	= 0;

	pDesc.pRootSignature		= m_rootSignature.Get();

	HRESULT hr = device->CreateGraphicsPipelineState(
		&pDesc, IID_PPV_ARGS(m_pso.GetAddressOf()));

	return SUCCEEDED(hr);
}

void Shader::setShader(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(m_pso.Get());
	cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
}