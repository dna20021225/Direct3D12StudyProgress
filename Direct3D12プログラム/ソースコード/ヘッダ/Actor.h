#pragma once

#include <d3d12.h>
#include"GMath.h"
#include"Mesh.h"
#include"GameUtil.h"
#include<memory>

class Actor
{
public:
	Actor(class Game* game, XMFLOAT3 pos = ZeroVec3d,
		float roll = 0.0f, float pitch = 0.0f, float yaw = 0.0f);
	virtual~Actor();

	virtual void update(float deltaTime) = 0;
	virtual void draw(ID3D12GraphicsCommandList* cmdList = nullptr) = 0;

	void setDead() { m_dead = true; }
	bool isDead()const { return m_dead; }

	void setPos(XMFLOAT3 pos);
	XMFLOAT3 getPos()const;
	void setRoll(float roll);
	float getRoll()const;
	void setPitch(float pitch);
	float getPitch()const;
	void setYaw(float yaw);
	float getYaw()const;

	XMFLOAT3 getForward()const;
	XMFLOAT3 getUp()const;
	XMFLOAT3 getRight()const;

	void setForwardSpeed(float s)	{ m_forwardSpeed = s; }
	float getForwardSpeed()const	{ return m_forwardSpeed; }
	void setUpSpeed(float s)		{ m_upSpeed = s; }
	float getUpSpeed()const			{ return m_upSpeed; }
	void setRightSpeed(float s)		{ m_rightSpeed = s; }
	float getRightSpeed()const		{ return m_rightSpeed; }

	void setRollSpeed(float s)		{ m_rollSpeed = s; }
	float getRollSpeed()const		{ return m_rollSpeed; }
	void setPitchSpeed(float s)		{ m_pitchSpeed = s; }
	float getPitchSpeed()const		{ return m_pitchSpeed; }
	void setYawSpeed(float s)		{ m_yawSpeed = s; }
	float getYawSpeed()const		{ return m_yawSpeed; }

	XMMATRIX getWorldMatrix() const { return m_worldTransform; }

	void setCurrentMaterial(MeshMaterial material);
	void setOriginalMaterial(MeshMaterial material);
	void resetMaterial();

	void setVisible(bool visible) { m_visible = visible; }
	bool isVisible() { return m_visible; }

protected:
	void calcWorldTransform();

	class Game* m_game;
	bool m_dead;
	bool m_visible;

	XMFLOAT3 m_pos;
	float m_roll;
	float m_pitch;
	float m_yaw;

	XMMATRIX m_worldTransform;
	bool m_recalculateWorldTransform;

	float m_forwardSpeed;
	float m_upSpeed;
	float m_rightSpeed;
	float m_rollSpeed;
	float m_pitchSpeed;
	float m_yawSpeed;

	std::unique_ptr<Mesh> m_mesh;



};

