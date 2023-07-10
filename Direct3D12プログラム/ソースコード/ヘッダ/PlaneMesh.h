#pragma once

#include "Mesh.h"

class PlaneMesh : public Mesh
{
public:
	PlaneMesh(class Actor* owner, class Renderer* renderer, UINT shaderIndex,
		const wchar_t* textureName, float sx = 1.0f, float sy = 1.0f,
		float su = 1.0f, float sv = 1.0f, MeshMaterial* meshMaterial = nullptr);
	~PlaneMesh();

	void draw(ID3D12GraphicsCommandList* cmdList) override;

protected:

};
