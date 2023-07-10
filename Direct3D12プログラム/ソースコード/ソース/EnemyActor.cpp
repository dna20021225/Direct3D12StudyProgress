#include"EnemyActor.h"
#include"Game.h"
#include<vector>

EnemyActor::EnemyActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName, float scale)
	:BoxActor(game, shaderIndex, XMFLOAT3(pos.x, pos.y, 0.5f*scale), textureName, true,
		scale, scale, scale)
	//, m_sphere(this, length(XMFLOAT3(0.5f * scale, 0.5f * scale, 0.5f * scale)))

{
	setOriginalMaterial(MeshMaterial(XMFLOAT3(1.0f, 0.2f, 0.2f),
		XMFLOAT3(0.08f, 0.08f, 0.08f), 1.0f, XMFLOAT3(0.1f, 0.1f, 0.1f)));

	/*std::vector<XMFLOAT3>points;
	float c = 0.5f * scale;
	points.push_back(XMFLOAT3(c, c, c));
	points.push_back(XMFLOAT3(-c, c, c));
	points.push_back(XMFLOAT3(c, -c, c));
	points.push_back(XMFLOAT3(c, c, -c));
	points.push_back(XMFLOAT3(-c, -c, c));
	points.push_back(XMFLOAT3(-c, c, -c));
	points.push_back(XMFLOAT3(c, -c, -c));
	points.push_back(XMFLOAT3(-c, -c, -c));*/
	m_box = AABBCollision(this, XMFLOAT3(scale, scale, scale),ZeroVec3d);

	setRollSpeed(50.0f);
	setYawSpeed(50.0f);
	setPitchSpeed(50.0f);
}

EnemyActor::~EnemyActor()
{

}

void EnemyActor::update(float deltaTime)
{
	// ƒAƒŒƒ“ƒW
	setRoll(m_roll + deltaTime * 1 * m_rollSpeed);
	setYaw(m_yaw + deltaTime * 1 * m_yawSpeed);
	setPitch(m_pitch + deltaTime * 1 * m_pitchSpeed);

	setPos(XMFLOAT3(getPos().x, getPos().y, 1.0f));
	
	calcWorldTransform();
}

void EnemyActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isEnabled())return;
	if (!isVisible())return;

	BoxActor::draw(cmdList);
}