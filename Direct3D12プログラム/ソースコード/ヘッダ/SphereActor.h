#pragma once

#include"Actor.h"
#include"SphereMesh.h"

#include<memory>

class SphereActor :public Actor
{
public:
	SphereActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textureName, bool outer, float radius,
		UINT nv, UINT nh, float su, float sv, MeshMaterial* material = nullptr);
	~SphereActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr) override;

	bool isEnabled() const;
private:

};