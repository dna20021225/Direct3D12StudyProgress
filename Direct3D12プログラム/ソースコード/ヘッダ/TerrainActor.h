#pragma once

#include"PlaneActor.h"
#include"Collision.h"

class TerrainActor :public PlaneActor
{
public:
	TerrainActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textureName);
	~TerrainActor();

	void update(float deltaTime) override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr)override;

	PlaneCollision getCollision()const { return m_plane; }
private:
	PlaneCollision m_plane;
};