#pragma once

#include "Mesh.h"

class HemisphereMesh :public Mesh
{
public:
	HemisphereMesh(class Actor* owner, class Renderer* renderer, UINT shaderIndex,
		const wchar_t* textureName, bool outer, float radius, UINT nv, UINT nh,
		float imgWidth, float imgHeight, float cx, float cy, float imgRadius,
		float angleScale, bool orthographic, MeshMaterial* material = nullptr);
	~HemisphereMesh();

	void draw(ID3D12GraphicsCommandList* cmdList) override;
protected:
	UINT m_indicesNum;
};