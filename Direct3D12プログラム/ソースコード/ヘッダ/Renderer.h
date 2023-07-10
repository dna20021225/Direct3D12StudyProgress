#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "DirectXTex.lib")

#include <wrl.h>
using namespace Microsoft::WRL;

#include <DirectXMath.h>
using namespace DirectX;

#include <memory>
#include <string>
#include <unordered_map>

#include "Camera.h"
#include "Shader.h"
#include "GameUtil.h"

class Renderer
{
public:
	Renderer(class Game* game, float r = 0.0f, float g = 0.0f, float b = 0.0f);
	~Renderer();

	bool initialize();
	void update();

	ID3D12GraphicsCommandList* begin();
	void end();

	//void draw();

	void setBackColor(float r, float g, float b);
	void setBackColor(float* color);
	const float* getBackColor()const { return m_backColor; }

	void setCameraExtParam(CamExtParam extparam);

	void setShader(UINT shaderIdx);

	int allocateConstBuffer(const void* pData, size_t dsize);
	void uploadConstBuffer(UINT idx, const void* pData, size_t dsize);
	void releaseConstBuffer(UINT idx);

	int allocateShaderResource(const wchar_t* filePath);

	int allocateVertexBuffer(void* pData, size_t dataSize, size_t stride);
	void releaseVertexBuffer(UINT idx);

	int allocateIndexBuffer(void* pData, size_t datasize);
	void releaseIndexBuffer(UINT idx);

	void setCommandConstantBufferView(UINT tableIdx, UINT idx);
	void setCommandShaderResourceView(UINT tableIdx, UINT idx);
	void setCommandVertexBufferView(UINT idx);
	void setCommandIndexBufferView(UINT idx);
	CamExtParam getCameraExtParam() const { return m_camera.getExtParam(); }
	XMMATRIX getCameraMatrix() const { return m_cameraMatrix; }
	ParalleLight getLight()const { return m_light; }
	void setLight(ParalleLight light);
	void setLight(XMFLOAT3 dir, XMFLOAT3 col);
	bool createUnicolorImage(const wchar_t* imageName, ColorRGBA unicolor);

private:
	bool createFactory();
	bool createDevice(const wchar_t* adapterName);
	bool createCommandQueue();
	bool createCommandAllocators();
	bool createCommandList();
	bool createSwapchain();
	bool createRenderTargetView(DXGI_FORMAT format);

	bool createFence();

	void moveToNextFrame();
	void waitForGPU();
	void setResourceBarrier(D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter);

	void enableDebugLayer();

	bool createResourceBuffer(ID3D12Resource** buffer, UINT64 bSize);
	bool uploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize);

	void setVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
		ID3D12Resource* buffer, UINT bSize, UINT stride);
	void setIndexBufferView(D3D12_INDEX_BUFFER_VIEW& indexBufferView,
		ID3D12Resource* index, UINT bSize);

	bool createShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
		DXGI_FORMAT format, UINT16 mipLevels = 1, UINT16 deptg0rArraySize = 1,
		D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D);
	bool uploadShaderResource(ID3D12Resource* buffer, void* src,
		UINT lineSize, UINT allSize);
	void setShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
		ID3D12DescriptorHeap* dHeap, UINT index);

	bool createDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum);
	bool readImageFile(ID3D12Resource** buffer, TexMetadata& metadata,
		const wchar_t* filePath, bool ddsFlag = false);

	void setViewport(D3D12_VIEWPORT& viewport);
	void setScissorRect(D3D12_RECT& scosspr);

	bool createConstBuffer(ID3D12Resource** buffer, const void* pData, size_t dsize,
		void** pmap, ID3D12DescriptorHeap* dHeap, UINT index);
	void setConstBufferView(ID3D12Resource* buffer,	ID3D12DescriptorHeap* dHeap,
		UINT index);

	UINT64 calcAlignment256(size_t size);

	XMMATRIX calcSpriteMatrix(const XMFLOAT2& p1, float w, float h);
	XMMATRIX calcSpriteMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2);

	XMMATRIX calcSpriteUVMatrix(const XMFLOAT2& p1, float w, float h, float imgW, float imgH);
	XMMATRIX calcSpriteUVMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float imgW, float imgH);

	bool createDepthBuffer(ID3D12Resource** buffer);
	bool createDepthHeap(ID3D12DescriptorHeap** heap, ID3D12Resource* depth);

	static const UINT FrameNum = 2;
	static const UINT ShaderNum = 6;
	static const UINT ShaderResourceNum = 1000;
	static const UINT ConstBufferNum = 50000;
	static const UINT SCViewNum = ShaderResourceNum + ConstBufferNum;
	static const UINT VertexBufferNum = 50000;
	static const UINT IndexBufferNum = 50000;
	static const int ReleaseCountStart = 5;

	class Game* m_game;

	D3D_FEATURE_LEVEL		m_featureLevel;
	ComPtr<IDXGIFactory4>	m_dxgiFactory;
	ComPtr<ID3D12Device>	m_device;

	ComPtr<ID3D12CommandAllocator>		m_cmdAllocators[FrameNum];
	ComPtr<ID3D12GraphicsCommandList>	m_cmdList;
	ComPtr<ID3D12CommandQueue>			m_cmdQueue;
	ComPtr<IDXGISwapChain3>				m_swapchain;
	UINT								m_bufferIndex;
	ComPtr<ID3D12DescriptorHeap>		m_rtvheap;
	ComPtr<ID3D12Resource>				m_backBuffers[FrameNum];
	ComPtr<ID3D12Resource>				m_depthBuffer;
	ComPtr<ID3D12DescriptorHeap>		m_depthHeap;

	UINT								m_rtvIncSize;
	UINT								m_csuIncSize;

	ComPtr<ID3D12Fence>					m_fence;
	UINT64								m_fenceValues[FrameNum];
	D3D12_VIEWPORT						m_viewport;
	D3D12_RECT							m_scissorRect;

	float								m_backColor[3];

	Camera								m_camera;
	XMMATRIX							m_cameraMatrix;
	std::unique_ptr<Shader>				m_shaders[ShaderNum];
	int									m_shaderIndex;

	ComPtr<ID3D12DescriptorHeap>		m_scDescHeap[FrameNum];
	ComPtr<ID3D12Resource>				m_shaderResource[ShaderResourceNum];
	ComPtr<ID3D12Resource>				m_constBuffer[FrameNum][ConstBufferNum];
	void*								m_constBufferMap[FrameNum][ConstBufferNum];
	std::unordered_map<std::wstring, UINT> m_shaderResourceList;
	std::vector<std::pair<UINT, int>>	m_constBufferReleaseList;
	ComPtr<ID3D12Resource>				m_vertexBuffer[VertexBufferNum];
	D3D12_VERTEX_BUFFER_VIEW			m_vertexBufferView[VertexBufferNum];
	std::vector<std::pair<UINT, int>>	m_vertexBufferReleaseList;
	ComPtr<ID3D12Resource>				m_indexBuffer[IndexBufferNum];
	D3D12_INDEX_BUFFER_VIEW				m_indexBufferView[IndexBufferNum];
	std::vector<std::pair<UINT, int>>	m_indexBufferReleaseList;

	ParalleLight m_light;

};