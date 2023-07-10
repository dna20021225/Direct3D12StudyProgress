#pragma once

#include <d3d12.h>
#include <vector>
#include "GameUtil.h"

struct BufferIndex
{
	int vertexID;
	int indexID;
	int shaderResourceID;

	BufferIndex() :vertexID(-1), indexID(-1), shaderResourceID(-1) {};
};

class Mesh
{
public:
	Mesh(class Actor* owner, class Renderer* renderer,
		UINT shaderIndex, MeshMaterial* material = nullptr);
	virtual ~Mesh();

	virtual void draw(ID3D12GraphicsCommandList* cmdList) = 0;

	bool isEnabled() const { return m_enabled; }

	void setCurrentMaterial(MeshMaterial material);
	void setOriginalMaterial(MeshMaterial material);
	void resetMaterial();

protected:
	void releaseBuffer(BufferIndex& buffer);
	void setConstData(ConstData& cdata);

	bool m_enabled;

	int m_constIndex;
	std::vector<BufferIndex> m_bufferIndex;

	class Actor* m_owner;
	class Renderer* m_renderer;
	UINT m_shaderIndex;

	MeshMaterial m_oriMaterial;
	MeshMaterial m_curMaterial;
};
