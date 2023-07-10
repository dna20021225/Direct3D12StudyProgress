#pragma once

#include"Actor.h"
#include"FileMesh.h"

#include<memory>

class FileMeshActor :public Actor
{
public:
	FileMeshActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const char* filePath, const char* imgFilePrefix,
		MeshMaterial* material = nullptr);
	~FileMeshActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr) override;

	bool isEnabled()const;

private:

};