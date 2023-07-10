#pragma once 
#include "Actor.h" 
#include "Camera.h" 

class CameraActor : public Actor

{
public:
	CameraActor(class Game* game, XMFLOAT3 pos, bool fpsCamera = true);
	~CameraActor();

	void update(float deltaTime) override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr) override;

	void setFPSCameraParam(float up);
	void setTPSCameraParam(float back, float backUp, float up, float front);

	void attach(Actor* actor, bool preActorVisible = true);
private:
	CamExtParam calcFPSCamera();
	CamExtParam calcTPSCamera();

	float m_fpsUpLength;	//FPS—p

	bool m_fpsCamera;
	//bool m_first;

	float m_tpsBackLength;	//TPS—p
	float m_tpsBackUpLength;
	float m_tpsUpLength;
	float m_tpsFrontLength;

	Actor* m_target;
};
