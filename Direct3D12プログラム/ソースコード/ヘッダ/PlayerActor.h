#pragma once

#include"Gamepad.h"
#include"Keyboard.h"
#include"FileMeshActor.h"
#include"Collision.h"

class PlayerActor :public FileMeshActor
{
public:
	PlayerActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos);
	~PlayerActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList)override;

	//float getRadius()const { return m_r; }
	AABBCollision getCollision()const { return m_box; }

private:
	//float m_r;
	AABBCollision m_box;
};