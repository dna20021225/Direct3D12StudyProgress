#include "Actor.h"
#include"Game.h"

Actor::Actor(Game* game, XMFLOAT3 pos,
	float roll, float pitch, float yaw)
	: m_game(game)
	, m_dead(false)
	, m_pos(pos)
	, m_roll(roll)
	, m_pitch(pitch)
	, m_yaw(yaw)
	, m_worldTransform(XMMatrixIdentity())
	, m_recalculateWorldTransform(true)
	, m_forwardSpeed(0.0f)
	, m_upSpeed(0.0f)
	, m_rightSpeed(0.0f)
	, m_rollSpeed(0.0f)
	, m_pitchSpeed(0.0f)
	, m_yawSpeed(0.0f)
	, m_visible(true)
{
}

Actor::~Actor()
{
}

void Actor::setPos(XMFLOAT3 pos)
{
	m_pos = pos;
	m_recalculateWorldTransform = true;
}

XMFLOAT3 Actor::getPos() const
{
	return m_pos;
}

void Actor::setRoll(float roll)
{
	m_roll = (roll > 360.0f) ? roll - 360.0f : ((roll < 0.0f) ? roll + 360.0f : roll);
	m_recalculateWorldTransform = true;
}

float Actor::getRoll() const
{
	return m_roll;
}

void Actor::setPitch(float pitch)
{
	m_pitch = (pitch > 360.0f) ? pitch - 360.0f : ((pitch < 0.0f) ? pitch + 360.0f : pitch);
	m_recalculateWorldTransform = true;
}

float Actor::getPitch() const
{
	return m_pitch;
}

void Actor::setYaw(float yaw)
{
	m_yaw = (yaw > 360.0f) ? yaw - 360.0f : ((yaw < 0.0f) ? yaw + 360.0f : yaw);
	m_recalculateWorldTransform = true;
}

float Actor::getYaw() const
{
	return m_yaw;
}

void Actor::calcWorldTransform()
{
	if (m_recalculateWorldTransform == false)return;

	m_recalculateWorldTransform = false;

	m_worldTransform = XMMatrixRotationX(XMConvertToRadians(m_roll));
	m_worldTransform *= XMMatrixRotationY(XMConvertToRadians(m_pitch));
	m_worldTransform *= XMMatrixRotationZ(XMConvertToRadians(m_yaw));
	m_worldTransform *= XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}

XMFLOAT3 Actor::getForward() const
{
	float rx = m_worldTransform.r[0].m128_f32[0];
	float ry = m_worldTransform.r[0].m128_f32[1];
	float rz = m_worldTransform.r[0].m128_f32[2];

	return XMFLOAT3(rx, ry, rz);
}

XMFLOAT3 Actor::getUp() const
{
	float rx = m_worldTransform.r[2].m128_f32[0];
	float ry = m_worldTransform.r[2].m128_f32[1];
	float rz = m_worldTransform.r[2].m128_f32[2];

	return XMFLOAT3(rx, ry, rz);
}

XMFLOAT3 Actor::getRight() const
{
	float rx = m_worldTransform.r[1].m128_f32[0];
	float ry = m_worldTransform.r[1].m128_f32[1];
	float rz = m_worldTransform.r[1].m128_f32[2];

	return XMFLOAT3(rx, ry, rz);
}

void Actor::setCurrentMaterial(MeshMaterial material)
{
	m_mesh->setCurrentMaterial(material);
}

void Actor::setOriginalMaterial(MeshMaterial material)
{
	m_mesh->setOriginalMaterial(material);
}

void Actor::resetMaterial()
{
	m_mesh->resetMaterial();
}