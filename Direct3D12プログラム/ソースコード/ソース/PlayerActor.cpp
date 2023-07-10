#include"PlayerActor.h"
#include"Game.h"
#include"BlockActor.h"

PlayerActor::PlayerActor(Game* game, UINT shaderIndex, XMFLOAT3 pos)
	:FileMeshActor(game, shaderIndex,pos, "Images\\model.gmesh", "Images\\")
{
	setOriginalMaterial(MeshMaterial(XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.01f, 0.01f, 0.01f), 1.0f, XMFLOAT3(0.3f, 0.3f, 0.3f)));

	float r = 0.3f;
	float h = 1.5f;
	m_box = AABBCollision(this, XMFLOAT3(2.0f * r, 2.0f * r, h), XMFLOAT3(0.0f, 0.0f, 0.5f * h));
}

PlayerActor::~PlayerActor()
{

}
void PlayerActor::update(float deltaTime)
{
	const Gamepad& gamepad = m_game->getGamepad();
	const Keyboard& keyboard = m_game->getKeyboard();
	int x = 0;
	int y = 0;
	if (gamepad.isDown(XINPUT_GAMEPAD_DPAD_UP))		x += 1;
	if (gamepad.isDown(XINPUT_GAMEPAD_DPAD_DOWN))	x -= 1;
	if (gamepad.isDown(XINPUT_GAMEPAD_DPAD_RIGHT))	y += 1;
	if (gamepad.isDown(XINPUT_GAMEPAD_DPAD_LEFT))	y -= 1;

	if (keyboard.isDown(VK_UP))		x += 1;
	if (keyboard.isDown(VK_DOWN))	x -= 1;
	if (keyboard.isDown(VK_RIGHT))	y += 1;
	if (keyboard.isDown(VK_LEFT))	y -= 1;

	if (x != 0)
	{
		setPos(m_pos + deltaTime * x * m_forwardSpeed * getForward());
	}
	else if (y != 0)
	{
		setYaw(m_yaw + deltaTime * y * m_yawSpeed);

		calcWorldTransform();
	}

	std::vector<BlockActor*> blocks = m_game->getBlocks();
	for (auto block = blocks.begin(); block != blocks.end(); ++block)
	{
		if (intersect(m_box, (*block)->getCollision()))
		{
			XMFLOAT3 fix = calcFixVector(m_box, (*block)->getCollision());
			setPos(m_pos + fix);
			calcWorldTransform();
		}
	}

	std::vector<EnemyActor*> enemies = m_game->getEnemies();
	for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
	{
		if ((*enemy)->isDead())continue;

		if (intersect(m_box, (*enemy)->getCollision()))
		{
			(*enemy)->setDead();
		}
	}

	if (gamepad.isPressed(XINPUT_GAMEPAD_B))
	{
		m_visible = !m_visible;
	}
	
	calcWorldTransform();
}

void PlayerActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isEnabled())return;
	if (!isVisible())return;

	FileMeshActor::draw(cmdList);
}