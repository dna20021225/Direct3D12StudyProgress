#pragma once

#include"BoxActor.h"
#include"Collision.h"

class BlockActor :public BoxActor
{
public:
	BlockActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textuerName, XMFLOAT3 size);
	~BlockActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList)override;

	//float getRadius()const { return m_r; }
	AABBCollision getCollision()const { return m_box; }

private:
	AABBCollision m_box;
};
