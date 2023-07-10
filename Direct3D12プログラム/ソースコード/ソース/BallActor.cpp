#include"BallActor.h"
#include"Game.h"
#include"TerrainActor.h"
#include"Gamepad.h"

BallActor::BallActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName, float radius)
	: SphereActor(game, shaderIndex, pos, textureName, true,
		radius, 30, 40, 1.0f, 1.0f)
	, m_sphere(this, radius)
	, m_move(false)
	, m_grounded(false)
	, m_basePos(pos)
{
	setOriginalMaterial(MeshMaterial(XMFLOAT3(0.95f, 0.5f, 0.0f),
		XMFLOAT3(0.08f, 0.08f, 0.08f), 1.0f, XMFLOAT3(0.1f, 0.1f, 0.1f)));
}

BallActor::~BallActor()
{

}

void BallActor::update(float deltaTime)
{
	if (m_move)
	{
		setPos(m_pos + deltaTime * m_upSpeed * UnitVecZ3d);

		TerrainActor* terrain = m_game->getTerrain();
		if (intersect(m_sphere, terrain->getCollision()))
		{
			//m_upSpeed = 0.0f;
			m_grounded = true;
			m_move = false;
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

void BallActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isEnabled())return;
	if (!isVisible())return;

	SphereActor::draw(cmdList);
}