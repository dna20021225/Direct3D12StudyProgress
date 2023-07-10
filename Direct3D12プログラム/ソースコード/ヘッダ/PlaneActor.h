#pragma once

#include "Actor.h"
#include "PlaneMesh.h"

#include <memory>

class PlaneActor : public Actor
{
public:
	PlaneActor(class Game* game, UINT shaderIndex, XMFLOAT3 pos,
		const wchar_t* textureName,
		float sx = 1.0f, float sy = 1.0f, float su = 1.0f, float sv = 1.0f,
		MeshMaterial * material = nullptr);
	~PlaneActor();

	void update(float deltaTime)override;
	void draw(ID3D12GraphicsCommandList* cmdList = nullptr) override;

	bool isEnabled() const;


private:
	//std::unique_ptr<PlaneMesh> m_plane;
};
