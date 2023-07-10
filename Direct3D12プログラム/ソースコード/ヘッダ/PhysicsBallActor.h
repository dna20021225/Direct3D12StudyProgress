#pragma once

#include"SphereActor.h"
#include"Collision.h"

class PhysicsBallActor :public SphereActor
{
public:
	PhysicsBallActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textureName, float radius);
	~PhysicsBallActor();

	void update(float deltaTime) override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr)override;

	SphpereColision getCollision()const { return m_sphere; }

private:
	SphpereColision m_sphere;
	bool m_move;
	bool m_grounded;
	XMFLOAT3 m_basePos;

	XMFLOAT3 m_velocity;
};