#pragma once

#include "Actor.h"
#include "BoxMesh.h"

#include <memory>
#include <vector>

class BoxActor : public Actor
{
public:
	BoxActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textureName, bool outer,
		float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
		MeshMaterial* material = nullptr);
	BoxActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		std::vector<const wchar_t*> textureNames, bool outer,
		float sx = 1.0f, float sy = 1.0f, float sz = 1.0f,
		MeshMaterial* material = nullptr);
	~BoxActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr) override;

	bool isEnabled() const;

private:

};
