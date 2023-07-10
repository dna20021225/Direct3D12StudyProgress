#pragma once

#include "Mesh.h"

class SphereMesh :public Mesh
{
public:
	SphereMesh(class Actor* owner, class Renderer* renderer, UINT shaderIndex,
		const wchar_t* textureName, bool outer, float radius, UINT nv, UINT nh,
		float su, float sv,	MeshMaterial* material = nullptr);
	~SphereMesh();

	void draw(ID3D12GraphicsCommandList* cmdList) override;

protected:
	UINT m_indicesNum;
};