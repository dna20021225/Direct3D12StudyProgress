
#include "Renderer.h"
#include "Game.h"
#include <vector>
#include <string> 

//#include <d3dcompiler.h>
//#pragma comment(lib, "d3dcompiler.lib") 

//#include <DirectXTex.h>
//#pragma comment(lib, "DirectXTex.lib") 

Renderer::Renderer(Game* game, float r, float b, float g)
	: m_game(game)
	, m_featureLevel()
	, m_bufferIndex(0)
	, m_rtvIncSize(0)
	, m_csuIncSize(0)
	, m_fenceValues()
	, m_viewport()
	, m_scissorRect()
	, m_cameraMatrix(XMMatrixIdentity())
	, m_camera(CamInParam(60.0f, 0.2f, 10000.0f), CamExtParam(XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)))
	, m_shaderIndex(-1)
	, m_constBufferMap{nullptr}
	, m_vertexBufferView{}
	, m_indexBufferView{}

{
	setBackColor(r, g, b);
}

Renderer::~Renderer()
{
	waitForGPU();

	//COM I—¹
	CoUninitialize();
}

bool Renderer::initialize()
{
	//COM ‰Šú‰»
	{
		HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	}

#ifdef _DEBUG 
	enableDebugLayer();
#endif

	if (!createFactory()) return false;
	if (!createDevice(L"NVIDIA")) return false;
	if (!createCommandQueue()) return false;
	if (!createSwapchain()) return false;
	if (!createCommandAllocators()) return false;
	if (!createCommandList()) return false;
	if (!createRenderTargetView(DXGI_FORMAT_R8G8B8A8_UNORM)) return false;


	if (!createFence()) return false;

	m_cmdList->Close();

	setViewport(m_viewport);
	setScissorRect(m_scissorRect);

	if (!createDepthBuffer(m_depthBuffer.GetAddressOf()))return false;
	if (!createDepthHeap(m_depthHeap.GetAddressOf(), m_depthBuffer.Get()))return false;

	D3D12_INPUT_ELEMENT_DESC inputLayouts[3];

	inputLayouts[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayouts[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayouts[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	m_shaders[0] = std::make_unique<Shader>(m_device.Get(), L"Shader\\GameVS.cso", L"Shader\\GamePS.cso",
		inputLayouts, 2, true, false, true);
	m_shaders[1] = std::make_unique<Shader>(m_device.Get(), L"Shader\\GameVS.cso", L"Shader\\GamePS.cso",
		inputLayouts, 2, true, false, false);
	m_shaders[2] = std::make_unique<Shader>(m_device.Get(), L"Shader\\SpriteVS.cso", L"Shader\\SpritePS.cso",
		inputLayouts, 2, true, false, true);
	m_shaders[3] = std::make_unique<Shader>(m_device.Get(), L"Shader\\BasicLightingVS.cso", L"Shader\\BasicLightingPS.cso",
		inputLayouts, 3, true, true, false);
	m_shaders[4] = std::make_unique<Shader>(m_device.Get(), L"Shader\\ImgProcNegpVS.cso", L"Shader\\ImgProcNegpPS.cso",
		inputLayouts, 2, true, false, false);
	m_shaders[5] = std::make_unique<Shader>(m_device.Get(), L"Shader\\ImgProcSepVS.cso", L"Shader\\ImgProcSepPS.cso",
		inputLayouts, 2, true, false, false);

	for (int i = 0; i < ShaderNum; ++i)
	{
		if (!m_shaders[i]->isEnabled())return false;
	}

	for (int i = 0; i < FrameNum; ++i)
	{
		if (!createDescHeap(m_scDescHeap[i].GetAddressOf(), SCViewNum))return false;
	}

	if (!createUnicolorImage(WHITE_TEXTRE, ColorRGBA(255, 255, 255, 255)))return false;
	if (!createUnicolorImage(BLACL_TEXTRE, ColorRGBA(0, 0, 0, 255)))return false;
	if (!createUnicolorImage(RED_TEXTRE, ColorRGBA(255, 0, 0, 255)))return false;
	if (!createUnicolorImage(GREEN_TEXTRE, ColorRGBA(0, 255, 0, 255)))return false;
	if (!createUnicolorImage(BLUE_TEXTRE, ColorRGBA(0, 0, 255, 255)))return false;
	if (!createUnicolorImage(YELLOW_TEXTRE, ColorRGBA(255, 255, 0, 255)))return false;
	if (!createUnicolorImage(CYAN_TEXTRE, ColorRGBA(0, 255, 255, 255)))return false;
	if (!createUnicolorImage(MAGENTA_TEXTRE, ColorRGBA(255, 0, 255, 255)))return false;

	return true;
}

ID3D12GraphicsCommandList* Renderer::begin()
{
	m_cmdAllocators[m_bufferIndex]->Reset();
	m_cmdList->Reset(m_cmdAllocators[m_bufferIndex].Get(), nullptr);

	setResourceBarrier(D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvH = m_rtvheap->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += m_bufferIndex * m_rtvIncSize;
	auto dsvH = m_depthHeap->GetCPUDescriptorHandleForHeapStart();
	m_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

	m_cmdList->ClearRenderTargetView(rtvH, m_backColor, 0, nullptr);
	m_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	m_cmdList->RSSetViewports(1, &m_viewport);
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);

	m_cameraMatrix = m_camera.calcViewProjMatrix(static_cast<float>(m_game->getWidth()),
		static_cast<float> (m_game->getHeight()));
	m_cmdList->SetDescriptorHeaps(1, m_scDescHeap[m_bufferIndex].GetAddressOf());

	m_shaderIndex = -1;
	return m_cmdList.Get();
}

void Renderer::end()
{
	setResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	m_cmdList->Close();

	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);

	m_swapchain->Present(1, 0);
	moveToNextFrame();
}

void Renderer::setBackColor(float r, float g, float b)
{
	m_backColor[0] = (r > 1.0f) ? 1.0f : ((r < 0.0f) ? 0.0f : r);
	m_backColor[1] = (g > 1.0f) ? 1.0f : ((g < 0.0f) ? 0.0f : g);
	m_backColor[2] = (b > 1.0f) ? 1.0f : ((b < 0.0f) ? 0.0f : b);
}

void Renderer::setBackColor(float* color)
{
	setBackColor(color[0], color[1], color[2]);
}

bool Renderer::createFactory()
{
	UINT debugFlag;
#ifdef _DEBUG
	debugFlag = DXGI_CREATE_FACTORY_DEBUG;
#else
	debugFlag = 0;
#endif

	HRESULT hr =
		CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::createDevice(const wchar_t* adapterName)
{
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* sa = nullptr;
	int i = 0;
	while (m_dxgiFactory->EnumAdapters(i, &sa) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(sa);
		++i;
	}

	sa = nullptr;
	for (auto adp : adapters)
	{
		DXGI_ADAPTER_DESC adpDesc = {};
		adp->GetDesc(&adpDesc);
		std::wstring str = adpDesc.Description;
		if (str.find(adapterName) != std::wstring::npos)
		{
			sa = adp;
			break;
		}
	}

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	for (auto level : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(sa, level,
			IID_PPV_ARGS(m_device.GetAddressOf()))))
		{
			m_featureLevel = level;
			break;
		}
	}

	for (auto adp : adapters)
	{
		adp->Release();
	}
	if (m_device.Get() == nullptr) return false;

	m_rtvIncSize =
		m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_csuIncSize =
		m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

bool Renderer::createCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hr =
		m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_cmdQueue.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::createCommandAllocators()
{
	for (UINT i = 0; i < FrameNum; ++i)
	{
		HRESULT hr = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_cmdAllocators[i].GetAddressOf()));
		if (FAILED(hr)) return false;
	}

	return true;
}

bool Renderer::createCommandList()
{
	HRESULT hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_cmdAllocators[m_bufferIndex].Get(), nullptr,
		IID_PPV_ARGS(m_cmdList.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::createSwapchain()
{
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = m_game->getWidth();
	scDesc.Height = m_game->getHeight();
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo = FALSE;
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount = FrameNum;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
	fsDesc.Windowed = TRUE;

	HWND hwnd = m_game->getHwnd();
	ComPtr<IDXGISwapChain1> swapchain;
	HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(
		m_cmdQueue.Get(), hwnd, &scDesc, &fsDesc,
		nullptr, swapchain.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = swapchain.As(&m_swapchain);
	if (FAILED(hr)) return false;
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();

	hr = m_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	return SUCCEEDED(hr);
}

bool Renderer::createRenderTargetView(DXGI_FORMAT format)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = FrameNum;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	HRESULT hr = m_device->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(m_rtvheap.GetAddressOf()));
	if (FAILED(hr)) return false;

	auto handle = m_rtvheap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < FrameNum; ++i)
	{
		hr =
			m_swapchain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf()));
		if (FAILED(hr)) return false;

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = format;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		m_device->CreateRenderTargetView(m_backBuffers[i].Get(), &viewDesc, handle);
		handle.ptr += m_rtvIncSize;
	}

	return true;
}

bool Renderer::createFence()
{
	for (UINT i = 0; i < FrameNum; ++i)
	{
		m_fenceValues[i] = 0;
	}

	HRESULT hr = m_device->CreateFence(
		m_fenceValues[m_bufferIndex],
		D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf()));
	if (FAILED(hr)) return false;

	m_fenceValues[m_bufferIndex] ++;

	return true;
}

void Renderer::moveToNextFrame()
{
	auto currentValue = m_fenceValues[m_bufferIndex];
	m_cmdQueue->Signal(m_fence.Get(), currentValue);

	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();

	if (m_fence->GetCompletedValue() < m_fenceValues[m_bufferIndex])
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		if (event)
		{
			m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
			WaitForSingleObjectEx(event, INFINITE, FALSE);
			CloseHandle(event);
		}
	}

	m_fenceValues[m_bufferIndex] = currentValue + 1;
}

void Renderer::waitForGPU()
{
	m_cmdQueue->Signal(m_fence.Get(), m_fenceValues[m_bufferIndex]);
	HANDLE event = CreateEvent(nullptr, false, false, nullptr);
	if (event)
	{
		m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
		WaitForSingleObjectEx(event, INFINITE, FALSE);
		CloseHandle(event);
	}
	m_fenceValues[m_bufferIndex] ++;
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();
}
void Renderer::setResourceBarrier(D3D12_RESOURCE_STATES stateBefore,
	D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER desc = {};
	desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	desc.Transition.pResource = m_backBuffers[m_bufferIndex].Get();
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	desc.Transition.StateBefore = stateBefore;
	desc.Transition.StateAfter = stateAfter;

	m_cmdList->ResourceBarrier(1, &desc);
}

void Renderer::enableDebugLayer()
{
	ComPtr<ID3D12Debug> debugLayer;
	D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.GetAddressOf()));

	debugLayer->EnableDebugLayer();
}

bool Renderer::createResourceBuffer(ID3D12Resource** buffer, UINT64 bSize)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = bSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = m_device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::uploadResourceBuffer(ID3D12Resource* buffer,
	void* src, size_t bSize)
{
	void* map = nullptr;
	HRESULT hr = buffer->Map(0, nullptr, &map);
	if (FAILED(hr)) return false;

	if (map) memcpy(map, src, bSize);

	buffer->Unmap(0, nullptr);

	return true;
}

void Renderer::setVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
	ID3D12Resource* buffer, UINT bSize, UINT stride)
{
	vertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = bSize;
	vertexBufferView.StrideInBytes = stride;
}

void Renderer::setIndexBufferView(D3D12_INDEX_BUFFER_VIEW& indexBufferView,
	ID3D12Resource* index, UINT bSize)
{
	indexBufferView.BufferLocation = index->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = bSize;
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;
}

bool Renderer::createShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
	DXGI_FORMAT format, UINT16 miplevels, UINT16 depthorArraySize,
	D3D12_RESOURCE_DIMENSION dimension)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = dimension;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = depthorArraySize;
	desc.MipLevels = miplevels;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	HRESULT hr = m_device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::uploadShaderResource(ID3D12Resource* buffer, void* src,
	UINT lineSize, UINT allSize)
{
	HRESULT hr = buffer->WriteToSubresource(0, nullptr,
		src, lineSize, allSize);

	return SUCCEEDED(hr);
}

void Renderer::setShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
	ID3D12DescriptorHeap* dHeap, UINT index)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;

	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_csuIncSize * index;
	m_device->CreateShaderResourceView(buffer, &desc, handle);
}

bool Renderer::createDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = dNum;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT hr = m_device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(dHeap));
	return SUCCEEDED(hr);
}

bool Renderer::readImageFile(ID3D12Resource** buffer, TexMetadata& metadata,
	const wchar_t* filePath, bool ddsFlag)
{
	ScratchImage img = {};

	HRESULT hr;
	if (ddsFlag)
	{
		hr = LoadFromDDSFile(filePath, DDS_FLAGS_NONE, &metadata, img);
	}
	else
	{
		hr = LoadFromWICFile(filePath, WIC_FLAGS_NONE, &metadata, img);
	}
	if (FAILED(hr)) return false;

	if (!createShaderResource(buffer, static_cast<UINT> (metadata.width),
		static_cast<UINT> (metadata.height), metadata.format,
		static_cast<UINT16> (metadata.mipLevels),
		static_cast<UINT16> (metadata.arraySize),
		static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension))) return false;

	auto image = img.GetImage(0, 0, 0);
	if (!uploadShaderResource(*buffer, image->pixels,
		static_cast<UINT> (image->rowPitch),
		static_cast<UINT> (image->slicePitch))) return false;

	return true;
}

void Renderer::setViewport(D3D12_VIEWPORT& viewport)
{
	int w = m_game->getWidth();
	int h = m_game->getHeight();

	viewport.Width = static_cast<float>(w);
	viewport.Height = static_cast<float>(h);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void Renderer::setScissorRect(D3D12_RECT& scissor)
{
	int w = m_game->getWidth();
	int h = m_game->getHeight();

	scissor.left = 0;
	scissor.top = 0;
	scissor.right = w;
	scissor.bottom = h;
}

bool Renderer::createConstBuffer(ID3D12Resource** buffer, const void* pData, size_t dsize,
	void** pmap, ID3D12DescriptorHeap* dHeap, UINT index)
{
	if (!createResourceBuffer(buffer, calcAlignment256(dsize))) return false;

	*pmap = nullptr;
	HRESULT hr = (*buffer)->Map(0, nullptr, pmap);
	if (FAILED(hr) || (*pmap) == nullptr) return false;

	memcpy(*pmap, pData, dsize);

	setConstBufferView(*buffer, dHeap, index);

	return true;
}

void Renderer::setConstBufferView(ID3D12Resource* buffer,
	ID3D12DescriptorHeap* dHeap, UINT index)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = buffer->GetGPUVirtualAddress();
	desc.SizeInBytes = static_cast<UINT> (buffer->GetDesc().Width);

	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * m_csuIncSize;

	m_device->CreateConstantBufferView(&desc, handle);
}

UINT64 Renderer::calcAlignment256(size_t size)
{
	UINT64 s = static_cast<UINT64> (size);

	return (size + 0xff) & ~0xff;
}

void Renderer::setCameraExtParam(CamExtParam extParam)
{
	m_camera.setExtParam(extParam);
}

XMMATRIX Renderer::calcSpriteMatrix(const XMFLOAT2& p1, float w, float h)
{
	XMMATRIX mat = XMMatrixIdentity();
	float width = (float)m_game->getWidth();
	float height = (float)m_game->getHeight();

	mat.r[0].m128_f32[0] = 2.0f * w / width;
	mat.r[1].m128_f32[1] = 2.0f * h / height;
	mat.r[3].m128_f32[0] = (2.0f * p1.x + w - width) / width;
	mat.r[3].m128_f32[1] = (-2.0f * p1.y - h + height) / height;

	return mat;
}

XMMATRIX Renderer::calcSpriteMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2)
{
	return calcSpriteMatrix(p1, p2.x, p2.y - p1.y);
}

XMMATRIX Renderer::calcSpriteUVMatrix(const XMFLOAT2& p1, float w, float h, float imgW, float imgH)
{
	XMMATRIX mat = XMMatrixIdentity();

	mat.r[0].m128_f32[0] = w / imgW;
	mat.r[1].m128_f32[1] = h / imgH;
	mat.r[3].m128_f32[0] = p1.x / imgW;
	mat.r[3].m128_f32[1] = p1.y / imgH;

	return mat;
}

XMMATRIX Renderer::calcSpriteUVMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float imgW, float imgH)
{
	return calcSpriteUVMatrix(p1, p2.x - p1.x, p2.y - p2.y, imgW, imgH);
}

bool Renderer::createDepthBuffer(ID3D12Resource** buffer)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = (UINT64)m_game->getWidth();
	desc.Height = (UINT64)m_game->getHeight();
	desc.DepthOrArraySize = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	HRESULT hr = m_device->CreateCommittedResource(&heapProp,
		D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr) ? true : false;
}

bool Renderer::createDepthHeap(ID3D12DescriptorHeap** heap, ID3D12Resource* depth)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	HRESULT hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap));
	if (FAILED(hr))return false;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_device->CreateDepthStencilView(depth, &dsvDesc,
		(*heap)->GetCPUDescriptorHandleForHeapStart());

	return true;
}

void Renderer::setShader(UINT shaerIdx)
{
	if (m_shaderIndex == shaerIdx)return;
	if (shaerIdx < 0 || shaerIdx >= ShaderNum)return;

	m_shaders[shaerIdx]->setShader(m_cmdList.Get());
	m_shaderIndex = shaerIdx;
}

void Renderer::update()
{
	{
		auto it = m_constBufferReleaseList.begin();
		while (it != m_constBufferReleaseList.end())
		{
			(*it).second -= -1;
			if ((*it).second <= 0)
			{
				UINT idx = (*it).first;
				for (int i = 0; i < FrameNum; ++i)
				{
					m_constBuffer[i][idx].Reset();
				}
				it = m_constBufferReleaseList.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	{
		auto it = m_vertexBufferReleaseList.begin();
		while (it != m_vertexBufferReleaseList.end())
		{
			(*it).second -= -1;
			if ((*it).second <= 0)
			{
				UINT idx = (*it).first;
				m_vertexBuffer[idx].Reset();
				it = m_vertexBufferReleaseList.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	{
		auto it = m_indexBufferReleaseList.begin();
		while (it != m_indexBufferReleaseList.end())
		{
			(*it).second -= -1;
			if ((*it).second <= 0)
			{
				UINT idx = (*it).first;
				m_indexBuffer[idx].Reset();
				it = m_indexBufferReleaseList.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

int Renderer::allocateConstBuffer(const void* pData, size_t dsize)
{
	int idx = 0;
	for (; idx < ConstBufferNum; ++idx)
	{
		if (m_constBuffer[0][idx].Get() == nullptr) break;
	}
	if (idx >= ConstBufferNum)return -1;

	for (int i = 0; i < FrameNum; ++i)
	{
		if (!createConstBuffer(m_constBuffer[i][idx].GetAddressOf(), pData, dsize,
			&m_constBufferMap[i][idx], m_scDescHeap[i].Get(),
			(UINT)(ShaderResourceNum + idx)))
		{
			if (i > 0)
			{
				for (int j = i - 1; j >= 0; j--)
				{
					m_constBuffer[j][idx].Reset();
				}
			}
			return -1;
		}
	}
	return idx;
}

void Renderer::uploadConstBuffer(UINT idx, const void* pData, size_t dsize)
{
	if (idx >= ConstBufferNum)return;

	memcpy(m_constBufferMap[m_bufferIndex][idx], pData, dsize);
}

void Renderer::releaseConstBuffer(UINT idx)
{
	if (idx >= ConstBufferNum)return;
	if (m_constBuffer[0][idx].Get() == nullptr)return;
	m_constBufferReleaseList.push_back(std::make_pair(idx, ReleaseCountStart));
}

int Renderer::allocateShaderResource(const wchar_t* filePath)
{
	auto it = m_shaderResourceList.find(filePath);
	if (it != m_shaderResourceList.end())
	{
		return (int)((*it).second);
	}

	UINT idx = 0;
	for (; idx < ShaderResourceNum; ++idx)
	{
		if (m_shaderResource[idx].Get() == nullptr) break;
	}
	if (idx >= ShaderResourceNum)return -1;

	TexMetadata metadata = {};
	if (!readImageFile(m_shaderResource[idx].GetAddressOf(),
		metadata, filePath, false))return -1;

	for (int i = 0; i < FrameNum; ++i)
	{
		setShaderResourceView(m_shaderResource[idx].Get(),
			metadata.format, m_scDescHeap[i].Get(), idx);
	}

	m_shaderResourceList[filePath] = idx;
	return (int)idx;
}

int Renderer::allocateVertexBuffer(void* pData, size_t dataSize, size_t stride)
{
	int idx = 0;
	for (; idx < VertexBufferNum; ++idx)
	{
		if (m_vertexBuffer[idx].Get() == nullptr) break;
	}
	if (idx >= VertexBufferNum)return -1;

	if (!createResourceBuffer(m_vertexBuffer[idx].GetAddressOf(),
		(UINT64)dataSize))return -1;
	if(!uploadResourceBuffer(m_vertexBuffer[idx].Get(),
		pData, (UINT64)dataSize))
	{
		m_vertexBuffer[idx].Reset();
		return -1;
	}
	setVertexBufferView(m_vertexBufferView[idx], m_vertexBuffer[idx].Get(),
		(UINT)dataSize, (UINT)stride);

	return idx;
}

void Renderer::releaseVertexBuffer(UINT idx)
{
	if (idx >= VertexBufferNum)return;
	if (m_vertexBuffer[idx].Get() == nullptr)return;
	m_vertexBufferReleaseList.push_back(std::make_pair(idx, ReleaseCountStart));
}

int Renderer::allocateIndexBuffer(void* pData, size_t dataSize)
{
	int idx = 0;
	for (; idx < IndexBufferNum; ++idx)
	{
		if (m_indexBuffer[idx].Get() == nullptr) break;
	}
	if (idx >= IndexBufferNum)return -1;

	if (!createResourceBuffer(m_indexBuffer[idx].GetAddressOf(),
		(UINT64)dataSize))return -1;
	if (!uploadResourceBuffer(m_indexBuffer[idx].Get(),
		pData, (UINT64)dataSize))
	{
		m_indexBuffer[idx].Reset();
		return -1;
	}
	setIndexBufferView(m_indexBufferView[idx],
		m_indexBuffer[idx].Get(),(UINT)dataSize);

	return idx;
}

void Renderer::releaseIndexBuffer(UINT idx)
{
	if (idx >= IndexBufferNum)return;
	if (m_indexBuffer[idx].Get() == nullptr)return;
	m_indexBufferReleaseList.push_back(std::make_pair(idx, ReleaseCountStart));
}

void Renderer::setCommandConstantBufferView(UINT tableIdx, UINT idx)
{
	auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += m_csuIncSize * (ShaderResourceNum + idx);
	m_cmdList->SetGraphicsRootDescriptorTable(tableIdx, handle);
}

void Renderer::setCommandShaderResourceView(UINT tableIdx, UINT idx)
{
	auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += m_csuIncSize * idx;
	m_cmdList->SetGraphicsRootDescriptorTable(tableIdx, handle);
}

void Renderer::setCommandVertexBufferView(UINT idx)
{
	m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView[idx]);
}

void Renderer::setCommandIndexBufferView(UINT idx)
{
	m_cmdList->IASetIndexBuffer(&m_indexBufferView[idx]);
}

void Renderer::setLight(ParalleLight light)
{
	m_light = light;
}

void Renderer::setLight(XMFLOAT3 dir, XMFLOAT3 col)
{
	m_light = ParalleLight(dir, col);
}

bool Renderer::createUnicolorImage(const wchar_t* imageName, ColorRGBA unicolor)
{
	auto it = m_shaderResourceList.find(imageName);
	if (it != m_shaderResourceList.end())
	{
		return false;
	}

	UINT idx = 0;
	for (; idx < ShaderResourceNum; ++idx)
	{
		if (m_shaderResource[idx].Get() == nullptr) break;
	}
	if (idx >= ShaderResourceNum)return false;

	std::vector<ColorRGBA> colors(4 * 4);
	for (ColorRGBA& color : colors)
	{
		color = unicolor;
	}

	if (!createShaderResource(m_shaderResource[idx].GetAddressOf(),
		4, 4, DXGI_FORMAT_B8G8R8A8_UNORM))return false;

	if (!uploadShaderResource(m_shaderResource[idx].Get(), colors.data(),
		(UINT)(sizeof(ColorRGBA) * 4), (UINT)(sizeof(ColorRGBA) * colors.size())))return false;

	for (int i = 0; i < FrameNum; ++i)
	{
		setShaderResourceView(m_shaderResource[idx].Get(),
			DXGI_FORMAT_B8G8R8A8_UNORM, m_scDescHeap[i].Get(), idx);
	}

	m_shaderResourceList[imageName] = idx;

	return true;
}