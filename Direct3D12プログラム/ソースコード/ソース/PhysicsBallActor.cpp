#include"PhysicsBallActor.h"
#include"Game.h"
#include"TerrainActor.h"
#include"Gamepad.h"

PhysicsBallActor::PhysicsBallActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName, float radius)
	: SphereActor(game, shaderIndex, pos, textureName, true,
		radius, 30, 40, 1.0f, 1.0f)
	, m_sphere(this, radius)
	, m_move(false)
	, m_grounded(false)
	, m_basePos(pos)
	, m_velocity(ZeroVec3d)
{
	setOriginalMaterial(MeshMaterial(XMFLOAT3(0.0f, 0.66f, 0.4f),
		XMFLOAT3(0.08, 0.08, 0.08), 1.0f, XMFLOAT3(0.1f, 0.1f, 0.1f)));
}

PhysicsBallActor::~PhysicsBallActor()
{

}

void PhysicsBallActor::update(float deltaTime)
{
	if (m_move)
	{
		// 修正オイラー法
		//XMFLOAT3 v1 = m_velocity;
		//XMFLOAT3 v2 = m_velocity - deltaTime * 9.8f * UnitVecZ3d;
		//setPos(m_pos + deltaTime * 0.5f * (v1 + v2));
		//m_velocity = v2;

		// シンプレックス法
		m_velocity += deltaTime * (-9.8f * UnitVecZ3d);
		setPos(m_pos + deltaTime * m_velocity);

		TerrainActor* terrain = m_game->getTerrain();
		if (intersect(m_sphere, terrain->getCollision()))
		{
			//m_upSpeed = 0.0f;
			m_grounded = true;
			m_move = false;
			m_velocity = ZeroVec3d;
		}
	}
	else if (!m_grounded)
	{
		const Gamepad& gamepad = m_game->getGamepad();
		if (gamepad.isPressed(XINPUT_GAMEPAD_Y))
		{
			m_move = true;
		}
	}
	else
	{
		const Gamepad& gamepad = m_game->getGamepad();
		if (gamepad.isPressed(XINPUT_GAMEPAD_Y))
		{
			setPos(m_basePos);
			m_grounded = false;
		}
	}
	calcWorldTransform();
}

void PhysicsBallActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isEnabled())return;
	if (!isVisible())return;

	SphereActor::draw(cmdList);
}