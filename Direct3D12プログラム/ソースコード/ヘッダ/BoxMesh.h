#pragma once

#include "Mesh.h"
#include <vector>

class BoxMesh : public Mesh
{
public:
	BoxMesh(class Actor* owner, class Renderer* renderer, UINT shaderIndex,
		const wchar_t* textureName, bool outer,
		float sx = 1.0f, float sy = 1.0f, float zu = 1.0f,
		MeshMaterial* material = nullptr);
	BoxMesh(class Actor* owner, class Renderer* renderer, UINT shaderIndex,
		std::vector<const wchar_t*>& textureNames, bool outer,
		float sx = 1.0f, float sy = 1.0f, float zu = 1.0f,
		MeshMaterial* material = nullptr);
	~BoxMesh();

	void draw(ID3D12GraphicsCommandList* cmdList) override;

protected:
	bool setVertex(bool outer, float sx, float sy, float sz);
	bool setIndex();

};