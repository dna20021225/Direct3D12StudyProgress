#pragma once

#include"BoxActor.h"
#include"Collision.h"

class EnemyActor :public BoxActor
{
public:
	EnemyActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textuerName, float scale);
	~EnemyActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList)override;

	//float getRadius()const { return m_r; }
	AABBCollision getCollision()const { return m_box; }

private:
	//float m_r;
	AABBCollision m_box;
};