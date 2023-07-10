#include "CameraActor.h"
#include "Game.h"
//#include "Keyboard.h"
#include "Renderer.h"
#include"Gamepad.h"

CameraActor::CameraActor(class Game* game, XMFLOAT3 pos, bool fpsCamera)
	:Actor(game, pos)
	, m_fpsUpLength(0.0f)
	, m_tpsBackLength(1.0f)
	, m_tpsBackUpLength(1.0f)
	, m_tpsUpLength(0.0f)
	, m_tpsFrontLength(1.0f)
	, m_target(nullptr)
	, m_fpsCamera(fpsCamera)
{
}

CameraActor::~CameraActor()
{
}

void CameraActor::update(float deltaTime)
{
	const Gamepad& gamepad = m_game->getGamepad();
	const Keyboard& keyboard = m_game->getKeyboard();

	Renderer* renderer = m_game->getRenderer();

	if (gamepad.isPressed(XINPUT_GAMEPAD_A) || keyboard.isPressed(VK_SHIFT))
	{
		m_fpsCamera = !m_fpsCamera;
		if (m_target)
		{
			if (m_fpsCamera)
			{
				m_target->setVisible(false);
			}
			else
			{
				m_target->setVisible(true);
			}
		}
	}

	if (m_target)
	{
		setPos(m_target->getPos());
		setRoll(m_target->getRoll());
		setPitch(m_target->getPitch());
		setYaw(m_target->getYaw());
	}

	
	calcWorldTransform();
	CamExtParam cam = (m_fpsCamera) ? calcFPSCamera() : calcTPSCamera();
	renderer->setCameraExtParam(cam);
}

void CameraActor::draw(ID3D12GraphicsCommandList* cmdList)
{
}

CamExtParam CameraActor::calcFPSCamera()
{
	XMFLOAT3 camPos = m_pos + (getUp() * m_fpsUpLength);
	return CamExtParam(camPos, camPos + getForward(), getUp());
}

CamExtParam CameraActor::calcTPSCamera()
{
	XMFLOAT3 camPos = m_pos - (getForward() * m_tpsBackLength) + (getUp() * m_tpsBackUpLength);
	XMFLOAT3 lookatPos = m_pos + (getUp() * m_tpsUpLength) + (getForward() * m_tpsFrontLength);
	return CamExtParam(camPos, lookatPos, getUp());
}

void CameraActor::setFPSCameraParam(float up)
{
	m_fpsUpLength = up;
}

void CameraActor::setTPSCameraParam(float back, float backUp, float up, float front)
{
	m_tpsBackLength = back;
	m_tpsBackUpLength = backUp;
	m_tpsUpLength = up;
	m_tpsFrontLength = front;
}

void CameraActor::attach(Actor* actor, bool preActorVisible)
{
	if (m_target)
	{
		m_target->setVisible(preActorVisible);
	}

	m_target = actor;
	if (m_target == nullptr)return;

	if (m_fpsCamera)
	{
		actor->setVisible(false);
	}
	else
	{
		actor->setVisible(true);
	}
}