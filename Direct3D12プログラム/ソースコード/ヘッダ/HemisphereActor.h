#pragma once

#include"Actor.h"
#include"HemisphereMesh.h"

#include<memory>

class HemisphereActor :public Actor
{
public:
	HemisphereActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textureName, bool outer, float radius,UINT nv, UINT nh, 
		float imgWidth, float imgHeight, float cx, float cy, float imgRadius,
		float angleScale=1.0f, bool orthographic=true,
		MeshMaterial* material = nullptr);
	~HemisphereActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr) override;

	bool isEnabled()const;

private:

};